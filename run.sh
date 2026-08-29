#!/bin/bash

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ACE_BIN="$SCRIPT_DIR/ace"

if ! command -v Xephyr >/dev/null 2>&1; then
    echo "Xephyr not found. Install it first (e.g. sudo pacman -S xorg-server-xephyr)."
    exit 1
fi

if [ ! -f "$ACE_BIN" ]; then
    echo "ace binary not found at $ACE_BIN. Build it first with 'make'."
    exit 1
fi

if [ ! -x "$ACE_BIN" ]; then
    echo "$ACE_BIN is not executable."
    exit 1
fi

Xephyr :1 -screen 1280x800 &
XEPHYR_PID=$!

sleep 1

DISPLAY=:1 "$ACE_BIN" &
ACE_PID=$!

echo "Xephyr running (pid $XEPHYR_PID), ace running (pid $ACE_PID) on DISPLAY=:1"
