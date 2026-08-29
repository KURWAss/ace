![Logo](assets/logo.png)
# ace

![License](https://img.shields.io/badge/license-GPLv3-blue.svg)
![C++](https://img.shields.io/badge/C%2B%2B-17-00599C.svg)
![Platform](https://img.shields.io/badge/platform-X11-lightgrey.svg)
![PRs welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg)

A minimal floating window manager for X11, written in C++.

## Features

- Maps new windows as soon as they are created
- Move a window: `Alt` + left mouse button + drag
- Resize a window: `Alt` + right mouse button + drag
- Quit the window manager: `Alt` + `Shift` + `Q`

## Project layout

```
ace/
├── include/           header files
│   └── window_manager.h
├── src/                source files
│   ├── main.cpp
│   └── window_manager.cpp
├── Makefile
├── README.md
└── CONTRIBUTING.md
```

## Dependencies

- g++ with C++17 support
- libX11 (headers and library)

On Arch Linux:

```
sudo pacman -S libx11
```

## Building

```
make
```

The `ace` binary will be produced in the project root.

## Running

For testing without leaving your current session, use Xephyr:

```
sudo pacman -S xorg-server-xephyr
Xephyr :1 -screen 1280x800 &
DISPLAY=:1 ./ace
```

Then, in that same `DISPLAY=:1`, you can launch any X11 application, e.g.:

```
DISPLAY=:1 xterm
```

To use `ace` as your main window manager, add this to `~/.xinitrc`:

```
exec /path/to/ace
```

and start your session with `startx`.

## License

This project is licensed under the GNU General Public License v3.0 (GPLv3).
See the `LICENSE` file for the full text.
