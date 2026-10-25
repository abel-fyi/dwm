dwm
===

A small, locally built dwm setup. `config.h` contains the configuration and
keybindings. `startdwm` launches dwm and updates the status bar.

Build and run
-------------

Run `make`, then launch `./startdwm` from an X session.

Added Keybindings
-----------

Super+e            Search emojis and copy one with xclip
Super+s            Take a full-screen screenshot with scrot
Super+Shift+s      Select an area to screenshot with scrot

The emoji picker needs dmenu and xclip. Screenshot keys need scrot.

Patches
----------

pertag
gaps
attachbottom
swaptags

Status bar
----------

1. 📅 Date and time
2. 🖥️ OS name
3. 🌐 Hostname
4. 👤 Username
