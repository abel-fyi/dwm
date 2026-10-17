dwm setup so good it don't need commits.

A lightweight personal dwm setup based on upstream dwm. It keeps the
stock default configuration separate from personal settings: dwm uses
config.h, while config.def.h remains the upstream default.

BUILD AND LAUNCH
----------------
Build in this directory:
    make

Start dwm and the bundled status bar:
    ./startdwm

The launcher runs the binaries from this directory, so dwm and its status
bar do not need to be installed system-wide.

PATCHES
-------
Patch files are included for reference and can be applied to the matching
upstream version:
    gaps-bca7cf2.diff
    pertag-bca7cf2.diff
    swaptags-bca7cf2.diff
    attachbottom-bca7cf2.diff

Swaptags also swaps the per-tag layouts.

PERSONAL KEYBINDINGS
--------------------
Super is the modifier key (Mod4).

Super+E              Emoji picker; copies the selected emoji
Super+R              Toggle Redshift color temperature
Super+S              Save a full screenshot
Super+Shift+S        Select an area and save a screenshot
Super+Ctrl+S         Select an area and copy it to the clipboard
Volume media keys    Raise, lower, or mute volume
Brightness keys      Raise or lower screen brightness
Alt+Shift+1–9        Swap the current tag with the selected tag

Screenshots are saved under ~/Pictures/Screenshots. Other dwm keybindings
use the standard dwm defaults.

STATUS BAR
----------
The bundled dwm-status program displays CPU and memory use, Wi-Fi, volume,
battery, date, and time. CPU and memory use emoji labels. Battery status
uses a low-battery icon at 25% and below.

TOOLS USED BY BINDINGS
----------------------
dmenu and xclip       Emoji picker
scrot                 Screenshots
redshift              Color temperature toggle
pactl                 Volume controls
brightnessctl         Brightness controls
alacritty             Terminal

Build requirements include a C compiler, make, Xlib development files, and
PulseAudio development files. The setup targets Linux and X11.
