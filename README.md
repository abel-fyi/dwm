# dwm

- small c status bar: date/time, cpu, ram, wifi ssid, pulseaudio volume and battery.
  refreshes every second; volume/mute changes appear immediately.
- volume/mute keys use pactl.
- brightness keys use brightnessctl (1% steps).
- super+r toggles redshift; requires redshift with randr support.
- nighttemp in config.h sets the temperature; default 3500 K.
- super+s saves the desktop to ~/Pictures/Screenshots/ with a timestamp.
- super+shift+s selects an area or window to save.
- super+ctrl+s selects an area or window to copy to the clipboard.
- screenshots require scrot; clipboard captures also require xclip.
- fixed gaps; no keybindings
- pertag
- swaptags (alt+shift+1–9; moves per-tag settings with the windows)
- super modifier
- super+shift+enter opens alacritty; change termcmd in config.h to use another terminal.
- resizehints = 0
- super+e searches emoji.txt with dmenu and copies the chosen emoji to the clipboard.
- bar and border alpha are separate config.h settings: 75% and 25% opacity by default.

build with `make`; install with `sudo make install`. start `dwm-status &`
before dwm in your x session.
build requires x11, xft, xinerama, xrender, fontconfig, freetype and libpulse headers.
volume needs a pulseaudio-compatible server (pulseaudio or pipewire-pulse).
customize config.h; config.def.h is stock. included patches are already applied.
patches apply in any order; swaptags needs pertag present when building.
emoji picker requires dmenu and xclip; config.h expects this repo at ~/src/dwm.
emoji display requires an emoji font, such as noto emoji.
bar and border translucency require a compositor; alpha-6.8.diff is already applied.
