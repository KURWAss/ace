#!/usr/bin/env bash

set -e

XSESSIONS_DIR="/usr/share/xsessions"

if [ "$EUID" -ne 0 ]; then
    echo "Please run this script as root (sudo)."
    exit 1
fi

if [ -f /usr/bin/ace ]; then
    rm -f /usr/bin/ace
    echo "Removed /usr/bin/ace"
else
    echo "/usr/bin/ace not found, skipping"
fi

if [ -f "$XSESSIONS_DIR/ace.desktop" ]; then
    rm -f "$XSESSIONS_DIR/ace.desktop"
    echo "Removed $XSESSIONS_DIR/ace.desktop"
else
    echo "$XSESSIONS_DIR/ace.desktop not found, skipping"
fi

echo "Done."
