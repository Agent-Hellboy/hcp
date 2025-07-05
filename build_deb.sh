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

# Capture relevant environment variables once
ENV_VARS=$(env | grep -E '^(XDG_SESSION_TYPE|DISPLAY|WAYLAND_DISPLAY|MIR_SOCKET|XAUTHORITY|DBUS_SESSION_BUS_ADDRESS|GDMSESSION|XDG_CURRENT_DESKTOP)=')

# Print them for debugging
echo "$ENV_VARS" | while IFS= read -r line; do
  echo "$line" >&2
done

echo "[hcp] Environment variables relevant to display server and session (set in this shell):" >&2
for var in XDG_SESSION_TYPE DISPLAY WAYLAND_DISPLAY MIR_SOCKET XAUTHORITY DBUS_SESSION_BUS_ADDRESS GDMSESSION XDG_CURRENT_DESKTOP; do
  value=$(echo "$ENV_VARS" | grep "^$var=" | cut -d'=' -f2-)
  if [ -n "$value" ]; then
    printf "  %s=%s\n" "$var" "$value" >&2
  fi
done

# Detect display server from captured environment
if echo "$ENV_VARS" | grep -q '^WAYLAND_DISPLAY='; then
  DETECTED_SERVER="wayland"
elif echo "$ENV_VARS" | grep -q '^MIR_SOCKET='; then
  DETECTED_SERVER="mir"
elif echo "$ENV_VARS" | grep -q '^XDG_SESSION_TYPE='; then
  DETECTED_SERVER="$(echo "$ENV_VARS" | grep '^XDG_SESSION_TYPE=' | cut -d'=' -f2)"
else
  DETECTED_SERVER="unknown"
fi

if { [ "${XDG_SESSION_TYPE:-}" != "x11" ] && [ -z "$DISPLAY" ]; } || [ -n "$WAYLAND_DISPLAY" ] || [ -n "$MIR_SOCKET" ]; then
  echo -e "\033[1;33m[hcp] WARNING: You are not running an X11 session.\033[0m"
  echo -e "\033[1;33m        Detected display server: $DETECTED_SERVER\033[0m"
  echo -e "\033[1;33m        We will add support for '$DETECTED_SERVER' later.\033[0m"
  echo -e "\033[1;33m        Please switch your display server to X11 until we figure out full support.\033[0m"
fi

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