# Dragon's Dogma: Dark Arisen dinput8.dll hook
## Features
### Save backups
A hook is placed just before the game creates a save. When it gets there, the current save gets duplicated with a timestamp added to it.  
If you need to revert to previous save, simply rename it to "ddda.sav".

It should find the save path automatically, if not see dinput8.ini.  
You can limit the number of backups it keeps in dinput8.ini.

### Character customization screen
Allows you to enter the character customization screen at any time.  
It replaces the Manual menu (the one on the very first screen when you launch the game).

You need to enter it with keyboard (press enter), doesn't work with mouse.

### In-game clock
Displays in-game time in top right corner of the screen.  
Should work with enb, may not work with steam overlay etc.

You need to enable it first, see dinput8.ini for additional settings.

### Cheats
Reduce your total weight  
Use town run everywhere  
     - forces the casual running/walking animation used inside of towns to be used outside of town as well (instead of the cautious one)  
     - it also removes the stamina drain while running

See dinput8.ini.

## Installation
Copy dinput8.dll and dinput8.ini into the main DDDA folder.

## Credits
MinHook - The Minimalistic x86/x64 API Hooking Library:  
https://github.com/TsudaKageyu/minhook

Idea for entering customization screen + forcing town run everywhere:  
http://forum.cheatengine.org/viewtopic.php?p=5641841#5641841