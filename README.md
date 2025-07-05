[![Build Status](https://github.com/Agent-Hellboy/hcp/actions/workflows/test.yml/badge.svg)](https://github.com/Agent-Hellboy/hcp/actions/workflows/test.yml)
[![Latest Release](https://img.shields.io/github/v/release/Agent-Hellboy/hcp?label=release)](https://github.com/Agent-Hellboy/hcp/releases)

# hcp

- A lightweight clipboard manager for Linux that stores your clipboard history, allowing you to access and reuse past entries much like how git stash works for code changes.
- Capture and browse clipboard entries: every time you copy (Ctrl+C) something, it’s saved. So if you’ve copied text 8 times, you can view and retrieve all 8 previous entries, just like accessing stashes in Git.

**Note:** This project currently targets POSIX systems only and uses APIs such as `mkdir`, `getenv`, `popen`, and `stat`. If there is significant user interest, I plan to migrate to cross-platform C++17 APIs in the future.


**Note:** This project currently only supports X11 display server. Wayland support has not been implemented yet.
```bash
echo $XDG_SESSION_TYPE
x11
```

## Installation

Download the latest `.deb` package from [the Releases page](https://github.com/Agent-Hellboy/hcp/releases) and install it with:
```sh
sudo dpkg -i hcp_1.0.0.deb
```

```
hcp 
hcp - Historical Clipboard Manager for X11

Usage:
  hcp service start        # Start clipboard monitoring service
  hcp list                 # List clipboard history
  hcp <index>              # Print clipboard entry at <index>
  hcp pop                  # Remove most recent clipboard entry
  hcp --help | -h          # Show this help message

Description:
  hcp is a lightweight clipboard manager for X11 systems. It captures clipboard entries,
  maintains a history, and allows you to list, print, or remove entries.
  Designed for reliability and minimalism, it works directly with the X11 clipboard
  and is suitable for use as a background service or on-demand.

```


## Usage

### Start the Clipboard Service
This will monitor your clipboard and save changes:
```sh
hcp service start &
```

or as a systemd service (Recommended):

(You will see instructions after the installation is finished.)
```bash
sudo dpkg -i hcp_1.0.0\(7\).deb 
(Reading database ... 316541 files and directories currently installed.)
Preparing to unpack hcp_1.0.0(7).deb ...
Unpacking hcp (1.0.0) over (1.0.0) ...
Setting up hcp (1.0.0) ...

After install, enable and start the service with:
  sudo systemctl daemon-reload
  sudo systemctl enable hcp@proshan.service
  sudo systemctl start hcp@proshan.service

```

if you follow it properly you would see something like below
```bash 
sudo systemctl status hcp@proshan.service
[sudo] password for proshan: 
● hcp@proshan.service - HCP Clipboard Manager Service
     Loaded: loaded (/etc/systemd/system/hcp@proshan.service; enabled; vendor preset: enabled)
     Active: active (running) since Fri 2025-07-04 02:50:03 IST; 6min ago
   Main PID: 3150329 (hcp)
      Tasks: 1 (limit: 38063)
     Memory: 292.0K
     CGroup: /system.slice/system-hcp.slice/hcp@proshan.service
             └─3150329 /usr/bin/hcp service start

Jul 04 02:50:03 twi-proshan systemd[1]: Started HCP Clipboard Manager Service.
```
### List Clipboard History
```sh
hcp list
```

### Print a Specific Clipboard Entry
Print the Nth entry (as shown in the list):
```sh
hcp <index>
# Example:
hcp 2
```

## How It Works
- Clipboard entries are saved in `~/.hcp/history.block` as binary blocks.
- Each entry is stored as: 4-byte length (uint32_t) followed by the clipboard content data.
- The format allows efficient appending and reading of the entire history in reverse chronological order.
- The service logs events to `~/.hcp/service.log`.

## Troubleshooting
- If `~/.hcp/history.block` is not created, check `~/.hcp/service.log` for errors.
- You can check service log using `sudo systemctl status hcp.service` 

## License
See [LICENSE](LICENSE).
