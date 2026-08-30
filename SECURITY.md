# Security Policy

## Reporting a Vulnerability

If you find a security issue in ace, please **do not open a public
issue**. Instead, report it privately by emailing:

**kurwa.offc@proton.me**

Please include:

- A description of the issue and its potential impact
- Steps to reproduce it (a minimal example config or sequence of events
  helps a lot)
- Your ace version or commit hash, and your distro/X server setup

You should receive a response within a few days. Once the issue is
confirmed, a fix will be prepared and a public advisory (crediting you,
unless you prefer to stay anonymous) will be published after the fix is
available.

## Scope

ace is an X11 window manager. Like any X11 client, it runs with the same
privileges as the user who started it and does not perform privilege
escalation. Relevant security concerns include (but aren't limited to):

- Ways a malicious or malformed X client (a window) could crash ace or
  make it misbehave (denial of service)
- Ways the config parser (`~/.config/ace/config`) could be tricked into
  executing something unintended
- Any input from other X11 clients that isn't properly validated before
  being used (window titles, properties, protocol messages)

Issues in third-party programs ace merely launches (a terminal, a
launcher, anything set via `terminal`, `apps`, or `execute` in the
config) are out of scope — please report those upstream instead.

## Non-Security Bugs

For anything that isn't a security concern, please use the regular
[issue tracker](https://github.com/KURWAss/ace/issues) instead of the
email above.
