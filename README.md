# Quake 4 Zombies
Midterm Project - Quake 4 Zombies Mod <br /> <br />
This mod gives Quake 4 a zombies-esque game mode. Think of Call of Duty Black Ops 3 with elements from other games. Player can choose from 5 pieces of equipment to fend off the zombie apocalypse, like barriers and turrets for example. Player can upgrade weapons and perks by scoring points as the game goes on. You earn 100 points per zombie killed.

## Screenshots
Player kills a wave of zombies to earn points. Automatic wave spawning has not been fully implemented. For now, players can press G on the keyboard to spawn as many zombies as they want. Armor HUD has been modified to reflect user points.
![image](https://user-images.githubusercontent.com/90282143/157355199-221c34be-cbdd-4061-a15f-87d1f19092e2.png)

Sgt. Morris is used as a shop for players to upgrade weapons, as well as buy perks and equipment. Shop GUI is not fully implemented. The wristcomm GUI is used instead for demonstration purposes.
![image](https://user-images.githubusercontent.com/90282143/157355336-c2af4e04-5b6e-4bff-a22d-e0cdf1c38fd9.png)

## Installation Instructions
Take the folder "q4zombies" and copy it into the folder containing Quake4.exe. Then, create a shortcut to automatically launch your mod. Do so by right-clicking Quake4.exe and go to Properties. Go to the end of the target line and enter the following: "+disconnect +set fs_game q4zombies". You can also put "+set com_allowConsole 1" in the target line to skip boot-up cutscenes and go straight to the main menu (if you want).
