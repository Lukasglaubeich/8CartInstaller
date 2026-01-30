# 8CartInstaller: A Pico-8 Cart Manager on 3DS
The Fake-08 Game Selection is not super friendly towards having many Games.
Thats what this app wants to change by giving a quick and easy way to manage
which games are in the p8carts folder and which arent.

## How to use:
In order to setup the app, place the .3dsx file somewhere on your SD Card and make a new Folder called "CARTS" (you can change this in code) in the root of your SD card.
Then place your Pico-8 Carts in there.
Also make a new Folder called p8carts in the root of your SD card.

Download the "Fake-08" App from the Universal Updater.
Now you are ready to go! Open the 8CartInstaller and Select the Games you like by pressing *A*. You can check and edit your selection by pressing R.
If you are done start the Fake-08 App and start Gaming!

## Features:
* Manage Pico-8 Games with ease
* Simple and User Friendly UI

## Controls:
* A: Move Game from Storage to p8carts or vice versa

* UP & DOWN: Navigate the File List

* R: Change from Storage view to p8carts view or vice versa

* START: Exit

## Where to get Games:
To fetch all the games from the official download site, use scripts like:
<https://github.com/icaroferre/Pico8-Cartloader> by @icaroferre

# Contributing:
I would very much appreciate a pull request if anyone wants to expand this or add their own ideas or even just fix small spellling mistakes!

## Adding Features:
Please create an issue befor you start to work so that others know what youre working on.

## Pull Requests:
When making a pull request, please keep commit messages clear and please code in a somewhat readable style.

If you dont know if you should make a code request, just do! Im very much a beginner myself! :)

# Compiling:
As for Build dependencies, you mostly need the same things you need to compile
the various examples in 3ds-examples: <https://github.com/devkitPro/3ds-examples>

### Planned Things:
* Make controls better (For example R and L instead of just R).
* Show the PNGs of the currently selected Cart on the Top Screen.
* Add a .cia file version.
* (Optional) fetch some info about the game on pressing y for example...
* (Optional) a Split View for Both SOURCE and DESTINATION.

### Credits:
* LodePNG: The library used to decode PNGs: <https://github.com/lvandeve/lodepng?tab=readme-ov-file>
* ctrmus: A music player of which I learned about and copied the File View System: <https://github.com/deltabeard/ctrmus>
* [Ctrulib](https://github.com/smealum/ctrulib)
