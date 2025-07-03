#!/bin/bash
set -e

VERSION=${VERSION:-1.0.0}
PKGDIR=hcp_$VERSION

# Clean up any previous build
rm -rf $PKGDIR
mkdir -p $PKGDIR/DEBIAN
mkdir -p $PKGDIR/usr/bin
mkdir -p $PKGDIR/etc/systemd/system

# Create control file
cat > $PKGDIR/DEBIAN/control <<EOF
Package: hcp
Version: $VERSION
Section: utils
Priority: optional
Architecture: amd64
Maintainer: Prince Roshan <princekrroshan01@gmail.com>
Description: HCP - Historical Clipboard Manager
 A historical clipboard manager with LRU history and systemd integration.
Depends: xclip
EOF

# Create postinst script directly
cat > $PKGDIR/DEBIAN/postinst <<'POSTINST'
#!/bin/sh
set -e

echo ""
echo "After install, enable and start the service with:"
echo "  sudo systemctl daemon-reload"
echo "  sudo systemctl enable hcp.service"
echo "  sudo systemctl start hcp.service"
echo ""
POSTINST
chmod 755 $PKGDIR/DEBIAN/postinst

# Build the binary
make
cp hcp $PKGDIR/usr/bin/hcp

# Copy files
cp hcp.service $PKGDIR/etc/systemd/system/hcp.service

# Build the .deb package
dpkg-deb --build $PKGDIR

echo "\nPackage built: $PKGDIR.deb (VERSION=$VERSION)"
echo "To install: sudo dpkg -i $PKGDIR.deb"
echo "If you see dependency errors, run: sudo apt-get install -f"
echo "After install, enable and start the service with:"
echo "  sudo systemctl daemon-reload"
echo "  sudo systemctl enable hcp.service"
echo "  sudo systemctl start hcp.service"
echo "\nYou can override the version by running: VERSION=your_version ./build_deb.sh" 