#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$SCRIPT_DIR"

echo "=== tdeDiskMark Build Script ==="

# Trinity DE installs its .pc files under /opt/trinity
export PKG_CONFIG_PATH="/opt/trinity/lib/pkgconfig:${PKG_CONFIG_PATH}"

# ---- Check build tools ----
MISSING=0
for tool in cmake make g++ pkg-config; do
    if ! command -v "$tool" &>/dev/null; then
        echo "  [MISSING] $tool"
        MISSING=1
    fi
done

# ---- Check required libraries via pkg-config ----
for lib in tqt tdelibs libnotify; do
    if ! pkg-config --exists "$lib" 2>/dev/null; then
        echo "  [MISSING] $lib"
        MISSING=1
    fi
done

# ---- Check for tqmoc (Trinity meta-object compiler) ----
if [ ! -x /usr/bin/tqmoc ]; then
    echo "  [MISSING] /usr/bin/tqmoc"
    MISSING=1
fi

if [ "$MISSING" -eq 1 ]; then
    echo ""
    echo "Some dependencies are missing."
    echo "On a Debian/Ubuntu system with the Trinity apt repository enabled, run:"
    echo ""
    echo "  sudo apt-get install cmake make g++ pkg-config \\"
    echo "       libtqtinterface-dev tdelibs14-trinity-dev libnotify-dev"
    echo ""
    echo "You may also want:  sudo apt-get install elfkickers   (for sstrip)"
    echo ""
    read -p "Attempt automatic install now? [y/N] " answer
    if [ "$answer" = "y" ] || [ "$answer" = "Y" ]; then
        sudo apt-get update
        sudo apt-get install -y cmake make g++ pkg-config \
            libtqtinterface-dev tdelibs14-trinity-dev libnotify-dev
    else
        echo "Aborting. Please install dependencies manually."
        exit 1
    fi
fi

echo "All dependencies found."
echo ""

# ---- Build ----
echo "--- Building ---"
mkdir -p build
cd build

cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

BIN="tdediskmark"

if [ ! -f "$BIN" ]; then
    echo "Build FAILED."
    exit 1
fi

echo ""
echo "Build successful."

# ---- Strip ----
if command -v sstrip &>/dev/null; then
    echo "Running sstrip for maximum size reduction..."
    sstrip "$BIN"
elif command -v strip &>/dev/null; then
    echo "sstrip not found, using strip..."
    strip -s -R .comment -R .gnu.version --strip-unneeded "$BIN" 2>/dev/null || true
fi

SIZE=$(du -h "$BIN" | cut -f1)
echo "Done.  build/$BIN  ($SIZE)"
