#!/bin/bash
set -e

VERSION=${VERSION:-1.0.0}
PKGDIR=hcp_$VERSION
USERNAME=$(logname 2>/dev/null || echo root)

# Clean up any previous build
rm -rf $PKGDIR
mkdir -p $PKGDIR/DEBIAN
mkdir -p $PKGDIR/usr/bin
mkdir -p $PKGDIR/etc/systemd/system

# Ensure build dependency is installed
apt-get update && apt-get install -y libx11-dev

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
Depends: libx11-6
EOF

# Create postinst script directly
cat > $PKGDIR/DEBIAN/postinst <<'POSTINST'
#!/bin/sh
set -e

USERNAME=$(logname 2>/dev/null || echo root)

cat > /etc/systemd/system/hcp@${USERNAME}.service <<EOF
[Unit]
Description=HCP Clipboard Manager Service
After=network.target

[Service]
Type=simple
User=${USERNAME}
Environment=DISPLAY=:0
Environment=XAUTHORITY=/home/${USERNAME}/.Xauthority
ExecStart=/usr/bin/hcp service start
Restart=on-failure

[Install]
WantedBy=multi-user.target
EOF

echo ""
echo "After install, enable and start the service with:"
echo "  sudo systemctl daemon-reload"
echo "  sudo systemctl enable hcp@${USERNAME}.service"
echo "  sudo systemctl start hcp@${USERNAME}.service"
echo ""
POSTINST
chmod 755 $PKGDIR/DEBIAN/postinst

# Build the binary
make
cp hcp $PKGDIR/usr/bin/hcp

# Build the .deb package
dpkg-deb --build $PKGDIR

echo "\nPackage built: $PKGDIR.deb (VERSION=$VERSION)"
echo "To install: sudo dpkg -i $PKGDIR.deb"
echo "If you see dependency errors, run: sudo apt-get install -f"
echo "After install, enable and start the service with:"
echo "  sudo systemctl daemon-reload"
echo "  sudo systemctl enable hcp@${USERNAME}.service"
echo "  sudo systemctl start hcp@${USERNAME}.service"
echo "\nYou can override the version by running: VERSION=your_version ./build_deb.sh" 