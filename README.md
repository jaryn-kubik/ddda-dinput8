# DDDASaveBackup
dinput8.dll hook for the game Dragon's Dogma: Dark Arisen, which backups saves.

## How does it work
Every time the game tries to create a save, the hook adds a timestamp to the file name.
(instead of "ddda.sav" it's for example "ddda_2016-01-20_20-18-59.sav")

Then whenever the game tries to load a save, the hook redirects it to the one with the latest timestamp.
-> When you need to revert to previous save, simply remove the new ones.

It also creates a log file (dinput8.log) in the main DDDA directory, which logs paths/names of handled saves.
(-> so you can see where to find them) 

## Installation
Copy dinput8.dll into the main DDDA folder.
