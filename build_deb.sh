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
Depends: libx11-6, libgtk-3-0, libappindicator3-1
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
echo "To check if your environment is set up correctly, run:"
echo "  hcp --diagnostic"
echo ""
echo "To enable and start the GUI tray app (clipboard history in system tray), run:"
echo "  sudo systemctl enable hcp-tray@${USERNAME}.service"
echo "  sudo systemctl start hcp-tray@${USERNAME}.service"
POSTINST
chmod 755 $PKGDIR/DEBIAN/postinst

# Build the main binary
make
cp hcp $PKGDIR/usr/bin/hcp

# Build the GUI tray app
cd gui && make && cd ..
cp gui/hcp-tray $PKGDIR/usr/bin/hcp-tray

# Add systemd service for tray app
cat > $PKGDIR/etc/systemd/system/hcp-tray@.service <<EOF
[Unit]
Description=HCP Clipboard Tray App
After=graphical-session.target

[Service]
Type=simple
User=%i
Environment=DISPLAY=:0
Environment=XAUTHORITY=/home/%i/.Xauthority
ExecStart=/usr/bin/hcp-tray
Restart=on-failure

[Install]
WantedBy=default.target
EOF

# Create the man page BEFORE building the .deb
mkdir -p $PKGDIR/usr/share/man/man1
cat > $PKGDIR/usr/share/man/man1/hcp.1 <<'EOF'
.TH hcp 1 "$(date +%Y-%m-%d)" "$VERSION" "hcp manual"
.SH NAME
hcp \- Historical Clipboard Manager for X11
.SH SYNOPSIS
.B hcp
[service start | list | <index> | pop | --help | -h]
.SH DESCRIPTION
.B hcp
is a lightweight clipboard manager for X11 systems. It captures clipboard entries, maintains a history, and allows you to list, print, or remove entries. Designed for reliability and minimalism, it works directly with the X11 clipboard and is suitable for use as a background service or on-demand.
.SH COMMANDS
.TP
.B service start
Start clipboard monitoring service in the background.
.TP
.B list
List clipboard history.
.TP
.B <index>
Print clipboard entry at <index>.
.TP
.B pop
Remove most recent clipboard entry.
.TP
.B --help, -h
Show this help message.
.SH AUTHOR
Written by the hcp project contributors.
.SH SEE ALSO
clipboard(1)
EOF
gzip -f $PKGDIR/usr/share/man/man1/hcp.1

# Build the .deb package
dpkg-deb --build $PKGDIR

echo "\nPackage built: $PKGDIR.deb (VERSION=$VERSION)"
echo "To install: sudo dpkg -i $PKGDIR.deb"
echo "If you see dependency errors, run: sudo apt-get install -f"
echo "After install, enable and start the service with:"
echo "  sudo systemctl daemon-reload"
echo "  sudo systemctl enable hcp@${USERNAME}.service"
echo "  sudo systemctl start hcp@${USERNAME}.service"
echo "To enable and start the GUI tray app (clipboard history in system tray), run:"
echo "  sudo systemctl enable hcp-tray@${USERNAME}.service"
echo "  sudo systemctl start hcp-tray@${USERNAME}.service"
echo ""
echo "You can override the version by running: VERSION=your_version ./build_deb.sh" 