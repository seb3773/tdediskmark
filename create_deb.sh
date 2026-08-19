#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$SCRIPT_DIR"

PKG_NAME="tdediskmark"
PKG_VERSION="1.0"
DEB_NAME="${PKG_NAME}_v${PKG_VERSION}.deb"
ARCH=$(dpkg --print-architecture 2>/dev/null || echo "amd64")

echo "=== tdeDiskMark .deb Package Builder ==="
echo "    Package : $DEB_NAME"
echo "    Arch    : $ARCH"
echo ""

# ---- Clean + Build ----
echo "--- Step 1: Clean ---"
./clean.sh
echo ""

echo "--- Step 2: Build ---"
./build.sh
echo ""

if [ ! -f "build/$PKG_NAME" ]; then
    echo "ERROR: build/$PKG_NAME not found. Build failed?"
    exit 1
fi

# ---- Assemble package tree ----
echo "--- Step 3: Package ---"
STAGING="$SCRIPT_DIR/build/deb_staging"
rm -rf "$STAGING"

# Binary
install -Dm755 "build/$PKG_NAME"      "$STAGING/usr/bin/$PKG_NAME"

# Desktop entry
install -Dm644 "data/$PKG_NAME.desktop" "$STAGING/usr/share/applications/$PKG_NAME.desktop"

# Icon — install the 64x64 original, symlink other sizes
ICON_SRC="icons/$PKG_NAME.png"
if [ ! -f "$ICON_SRC" ]; then
    echo "ERROR: $ICON_SRC not found."
    exit 1
fi

ICON_DIR_64="$STAGING/usr/share/icons/hicolor/64x64/apps"
install -Dm644 "$ICON_SRC" "$ICON_DIR_64/$PKG_NAME.png"

for SIZE in 16 22 24 32 48 128 256; do
    TARGET_DIR="$STAGING/usr/share/icons/hicolor/${SIZE}x${SIZE}/apps"
    mkdir -p "$TARGET_DIR"
    ln -sf "../../../../64x64/apps/$PKG_NAME.png" "$TARGET_DIR/$PKG_NAME.png"
done

# ---- DEBIAN control ----
mkdir -p "$STAGING/DEBIAN"

INSTALLED_SIZE=$(du -sk "$STAGING/usr" | cut -f1)

cat > "$STAGING/DEBIAN/control" << EOF
Package: $PKG_NAME
Version: $PKG_VERSION
Section: utils
Priority: optional
Architecture: $ARCH
Installed-Size: $INSTALLED_SIZE
Depends: fio, libnotify4 | libnotify-bin
Recommends: tdelibs14-trinity
Maintainer: seb3773 <seb3773@github.com>
Homepage: https://github.com/seb3773/tdediskmark
Description: Storage device benchmark tool for Trinity DE
 tdeDiskMark is a native TDE port of KDiskMark, the popular
 HDD/SSD benchmark tool. Features analog gauges with anti-aliased
 rendering, fun themes (Racing, Night Fly), desktop notifications,
 and multiple performance profiles. Uses fio under the hood.
EOF

# ---- Build .deb ----
dpkg-deb --build --root-owner-group "$STAGING" "build/$DEB_NAME"

rm -rf "$STAGING"

echo ""
echo "=== Package created: build/$DEB_NAME ==="
echo ""
echo "Install with:  sudo dpkg -i build/$DEB_NAME"
echo "Remove  with:  sudo dpkg -r $PKG_NAME"
