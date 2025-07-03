#!/bin/bash
set -e

VERSION=1.0.0
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
Maintainer: Prince Roshan <proshankrroshan01@gmail.com>
Description: HCP - Historical Clipboard Manager
 A historical clipboard manager with LRU history and systemd integration.
Depends: xclip
EOF

# Build the binary
make
cp hcp $PKGDIR/usr/bin/hcp

# Copy files
cp hcp.service $PKGDIR/etc/systemd/system/hcp@${USER}.service
chown -R root:root $PKGDIR

# Build the .deb package
dpkg-deb --build $PKGDIR

echo "\nPackage built: $PKGDIR.deb"
echo "To install: sudo dpkg -i $PKGDIR.deb"
echo "After install, enable and start the service with:"
echo "  sudo systemctl daemon-reload"
echo "  sudo systemctl enable hcp@${USER}.service"
echo "  sudo systemctl start hcp@${USER}.service" 