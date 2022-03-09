
#include "../../idlib/precompiled.h"
#pragma hdrstop

#include "../Game_local.h"

class rvMonsterSlimyTransfer : public idAI {
public:

	CLASS_PROTOTYPE( rvMonsterSlimyTransfer );

	rvMonsterSlimyTransfer ( void );

	void				InitSpawnArgsVariables			( void );
	void				Spawn							( void );
	void				Save							( idSaveGame *savefile ) const;
	void				Restore							( idRestoreGame *savefile );
	void				StartWave						( void ); // used for zombie waves

protected:

	virtual bool		CheckActions					( void );
	virtual void		OnDeath							( void );

	jointHandle_t		jointVomitMuzzle;

	int					vomitNextAttackTime;
	int					vomitAttackRate;

private:

	rvAIAction			actionVomitAttack;
	
	// Torso States
	stateResult_t		State_Torso_VomitAttack			( const stateParms_t& parms );
	stateResult_t		State_Torso_FinishVomitAttack	( const stateParms_t& parms );

	// Frame commands
	stateResult_t		State_Frame_StartVomit			( const stateParms_t& parms );
	stateResult_t		State_Frame_StopVomit			( const stateParms_t& parms );
	//void				Event_SetMoveSpeed(int speed);

	CLASS_STATES_PROTOTYPE ( rvMonsterSlimyTransfer );
};

CLASS_DECLARATION( idAI, rvMonsterSlimyTransfer )
END_CLASS

/*
================
rvMonsterSlimyTransfer::rvMonsterSlimyTransfer
================
*/
rvMonsterSlimyTransfer::rvMonsterSlimyTransfer ( void ) {
}

void rvMonsterSlimyTransfer::InitSpawnArgsVariables ( void ) {
	jointVomitMuzzle = animator.GetJointHandle ( spawnArgs.GetString ( "joint_vomitMuzzle", "puke_bone" ) );
	
	vomitAttackRate = SEC2MS ( spawnArgs.GetFloat ( "attack_vomit_rate", ".15" ) );
}

//void rvMonsterSlimyTransfer::Event_SetMoveSpeed(int speed) {
//	switch (speed) {
//	case AIMOVESPEED_DEFAULT:
//		move.fl.noRun = false;
//		move.fl.noWalk = false;
//		break;
//
//	case AIMOVESPEED_RUN:
//		move.fl.noRun = false;
//		move.fl.noWalk = true;
//		break;
//
//	case AIMOVESPEED_WALK:
//		move.fl.noRun = true;
//		move.fl.noWalk = false;
//		break;
//	}
//}

/*
================
rvMonsterSlimyTransfer::Spawn
================
*/

void rvMonsterSlimyTransfer::StartWave(void) {
	gameLocal.Printf("new wave approaching\n");
}

void rvMonsterSlimyTransfer::Spawn ( void ) {
	actionVomitAttack.Init ( spawnArgs, "action_vomitAttack", "Torso_VomitAttack", AIACTIONF_ATTACK );

	InitSpawnArgsVariables();
	//Event_SetMoveSpeed(1);
}

/*
================
rvMonsterSlimyTransfer::Save
================
*/
void rvMonsterSlimyTransfer::Save ( idSaveGame *savefile ) const {
	savefile->WriteInt( vomitNextAttackTime );

	actionVomitAttack.Save( savefile );
}

/*
================
rvMonsterSlimyTransfer::Restore
================
*/
void rvMonsterSlimyTransfer::Restore ( idRestoreGame *savefile ) {
	savefile->ReadInt( vomitNextAttackTime );

	actionVomitAttack.Restore( savefile );

	InitSpawnArgsVariables();
}

/*
================
rvMonsterSlimyTransfer::CheckActions
================
*/
bool rvMonsterSlimyTransfer::CheckActions ( void ) {
	if ( PerformAction ( &actionVomitAttack, (checkAction_t)&idAI::CheckAction_RangedAttack, &actionTimerRangedAttack ) ) {
		return true;
	}
	return idAI::CheckActions ( );
}

/*
================
rvMonsterSlimyTransfer::OnDeath
================
*/
void rvMonsterSlimyTransfer::OnDeath ( void ) {
	idPlayer* player = gameLocal.GetLocalPlayer();
	gameLocal.Printf("starting points: %i\n", player->inventory.points);
	
	StopEffect ( "fx_vomit_muzzle" );
	idAI::OnDeath ( );

	gameLocal.Printf("slimy transfer killed\n");
	// add user points
	player->inventory.points += 100;
	gameLocal.Printf("current points: %i\n", player->inventory.points);

}


/*
===============================================================================

	States 

===============================================================================
*/

CLASS_STATES_DECLARATION ( rvMonsterSlimyTransfer )
	STATE ( "Torso_VomitAttack",		rvMonsterSlimyTransfer::State_Torso_VomitAttack )
	STATE ( "Torso_FinishVomitAttack",	rvMonsterSlimyTransfer::State_Torso_FinishVomitAttack )

	STATE ( "Frame_StartVomit",			rvMonsterSlimyTransfer::State_Frame_StartVomit )
	STATE ( "Frame_StopVomit",			rvMonsterSlimyTransfer::State_Frame_StopVomit )
END_CLASS_STATES

/*
================
rvMonsterSlimyTransfer::State_Torso_VomitAttack
================
*/
stateResult_t rvMonsterSlimyTransfer::State_Torso_VomitAttack ( const stateParms_t& parms ) {	
	enum {
		STAGE_INIT,
		STAGE_WAIT,
	};	
	switch ( parms.stage ) {
		case STAGE_INIT:
			DisableAnimState ( ANIMCHANNEL_LEGS );

			vomitNextAttackTime = 0;

			// Loop the flame animation
			PlayAnim( ANIMCHANNEL_TORSO, "vomit_attack", parms.blendFrames );

			// Make sure we clean up some things when this state is finished (effects for one)			
			PostAnimState ( ANIMCHANNEL_TORSO, "Torso_FinishVomitAttack", 0, 0, SFLAG_ONCLEAR );
			
			return SRESULT_STAGE ( STAGE_WAIT );
		
		case STAGE_WAIT:
			if ( AnimDone ( ANIMCHANNEL_TORSO, parms.blendFrames ) ) {
				return SRESULT_DONE;
			}
			
			if ( vomitNextAttackTime && gameLocal.time >= vomitNextAttackTime ) {
				Attack ( "vomit", jointVomitMuzzle, enemy.ent );
				vomitNextAttackTime = gameLocal.time + vomitAttackRate;
			}
			
			return SRESULT_WAIT;			
	}

	return SRESULT_ERROR;
}

/*
================
rvMonsterSlimyTransfer::State_Torso_FinishVomitAttack
================
*/
stateResult_t rvMonsterSlimyTransfer::State_Torso_FinishVomitAttack ( const stateParms_t& parms ) {	
	State_Frame_StopVomit ( parms );
	return SRESULT_DONE;
}

/*
================
rvMonsterSlimyTransfer::State_Frame_StartVomit
================
*/
stateResult_t rvMonsterSlimyTransfer::State_Frame_StartVomit ( const stateParms_t& parms ) {	
	PlayEffect ( "fx_vomit_muzzle", jointVomitMuzzle, true );
	vomitNextAttackTime = gameLocal.time;	
	return SRESULT_DONE;
}

/*
================
rvMonsterSlimyTransfer::State_Frame_StopVomit
================
*/
stateResult_t rvMonsterSlimyTransfer::State_Frame_StopVomit ( const stateParms_t& parms ) {	
	StopEffect ( "fx_vomit_muzzle" );
	vomitNextAttackTime = 0;
	return SRESULT_DONE;
}
