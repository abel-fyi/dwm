# dwm 6.8

- small c status bar: date/time, cpu, ram, wifi ssid and pulseaudio volume.
  refreshes every second; volume/mute changes appear immediately.
- vol/mute and brightness bindings (pactl and brightnessctl).
- fixed gaps; no keybindings
- pertag
- swaptags (alt+shift+1–9)
- super modifier
- resizehints = 0
- super+e searches emoji.txt with dmenu and copies the chosen emoji to the clipboard.

build with `make`; install with `sudo make install`. start `dwm-status &`
before dwm in your x session. requires x11/xft/xinerama and libpulse headers.
customize config.h; config.def.h is stock. included patches are already applied.
emoji picker requires dmenu and xclip; config.h expects this repo at ~/src/dwm.
