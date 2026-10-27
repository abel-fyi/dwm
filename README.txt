dwm
===

A small, locally built dwm setup. `config.h` contains the configuration and
keybindings. `startdwm` launches dwm and updates the status bar.

Build and run
-------------

Run `make`, then run `./startdwm` from an X session.

Added Keybindings
-----------

Super+e            Search emojis and copy one with xclip
Super+s            Take a full-screen screenshot with scrot
Super+Shift+s      Select an area to screenshot with scrot
Super+r            Toggle a 4500K screen tint with redshift
Brightness up/down Adjust screen brightness by 5% on Linux (brightnessctl)
Volume up/down     Adjust default output volume by 5% on Linux (wpctl)

The emoji picker needs dmenu and xclip. Screenshot keys need scrot.
Linux brightness keys need brightnessctl and working brightness-key events.
Linux volume keys need PipeWire with WirePlumber (`wpctl`) and working volume-key events.
The toggle state starts off when dwm starts; if a tint remains, run `redshift -x` to reset it.

Patches
----------

pertag
gaps
attachbottom
swaptags

Status bar
----------

1. 🖥️ OS name
2. 🌐 Hostname
3. 👤 Username
4. 📡 Reply status from 8.8.8.8, checked about every 30 seconds
5. 🔋 Battery percentage on Linux, when a battery is found
6. 📅 Date and time

A ping reply is only a rough internet-connectivity signal; no reply can also mean
ICMP is blocked. The bar stays minimal and uses common Unix commands, keeping
it portable across Unix-like systems, including the BSDs.

`startdwm` sets the root background and launches dwm. One loop updates the clock
and bar every second, starts a background network check about every 30
seconds, and reads Linux battery status when the clock minute changes. A slow
ping leaves the last network result displayed while the clock keeps ticking;
probes never overlap. Comment out `linux_battery_enabled=1` near the top of
`startdwm` to hide the battery field; it is omitted on other systems. Change
`ping_interval` near the top of the script to adjust the network check interval.

Portability
-----------

The launcher and shared status fields aim to work across Unix-like systems.
Keep OS-specific status additions isolated, guarded by the OS, and easy to
toggle. The Linux battery field is the current example.
