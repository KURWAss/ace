#!/usr/bin/env bash

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BINARY_PATH="$SCRIPT_DIR/../ace"
DESKTOP_FILE="$SCRIPT_DIR/ace.desktop"
XSESSIONS_DIR="/usr/share/xsessions"

if [ "$EUID" -ne 0 ]; then
    echo "Please run this script as root (sudo)."
    exit 1
fi

if [ ! -f "$BINARY_PATH" ]; then
    echo "Binary not found at $BINARY_PATH. Build it first with 'make'."
    exit 1
fi

install -m 755 "$BINARY_PATH" /usr/bin/ace
echo "Installed /usr/bin/ace"

if [ ! -f "$DESKTOP_FILE" ]; then
    echo "Desktop file not found at $DESKTOP_FILE."
    exit 1
fi

mkdir -p "$XSESSIONS_DIR"
install -m 644 "$DESKTOP_FILE" "$XSESSIONS_DIR/ace.desktop"
echo "Installed $XSESSIONS_DIR/ace.desktop"

echo "Done. Select ace from your display manager session list."
