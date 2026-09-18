# Personal dwm build

This repository is a personalized build of dwm based on the official suckless
history. See `PATCHES.md` for the integrated changes.

## Dependencies

- Xlib, Xft, Xinerama, and Xrender development files
- the matching local `dmenu` and `st` builds
- `curl` and `xclip` for the emoji picker
- Noto Color Emoji

## Build and install

Install the sibling `dmenu` and `st` repositories first, then run:

```sh
make
sudo make install
```

Restart dwm after installation. `Mod+p` opens the launcher and `Mod+e` opens
the searchable emoji picker.

The official upstream should be configured as the `upstream` remote; reserve
`origin` for the personal GitHub repository.
