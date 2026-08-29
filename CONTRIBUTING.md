# Contributing to ace

Thanks for your interest in the project. ace is still in an early stage, so
the guidelines below may change over time.

## How to propose changes

1. Open an issue describing the problem or idea before writing code — this
   helps avoid duplicated work.
2. Fork the repository and create a separate branch for your change.
3. Keep commits small and logically self-contained.
4. Open a pull request describing what changed and why.

## Code style

- The project is written in C++17.
- Comments and identifiers must be in English.
- File and function names should be descriptive, without prefixes like
  `proj_`.
- Header files go in `include/`, source files go in `src/`.
- Before opening a PR, make sure `make` builds without warnings (the
  Makefile enables `-Wall -Wextra`).
- Formatting follows the Google C++ Style Guide: 4-space indentation,
  opening braces on the same line, `snake_case` for variables, `PascalCase`
  for classes and methods.

## Testing changes

Since ace is a window manager, breaking your main X session can be
disruptive. Test changes in Xephyr (see README.md) before using a build as
your primary window manager.

## Areas open for contribution

Current priorities:

- basic stability of the floating mode;
- multi-monitor support;
- a configuration file for keybindings.

If you want to pick up one of these, please open an issue first so we can
coordinate.
