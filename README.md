[![Build Status](https://github.com/Agent-Hellboy/hcp/actions/workflows/test.yml/badge.svg)](https://github.com/Agent-Hellboy/hcp/actions/workflows/test.yml)
[![Latest Release](https://img.shields.io/github/v/release/Agent-Hellboy/hcp?label=release)](https://github.com/Agent-Hellboy/hcp/releases)

# hcp

- A lightweight clipboard manager for Linux that stores your clipboard history, allowing you to access and reuse past entries much like how git stash works for code changes.
- Capture and browse clipboard entries: every time you copy (Ctrl+C) something, it's saved. So if you've copied text 8 times, you can view and retrieve all 8 previous entries, just like accessing stashes in Git.

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

## Development

To build the .deb package from source, run:

```sh
sudo  ./build_deb.sh
```

### After Installation: Check Your Environment
After installing, you should run the following command to check if your environment is set up correctly for clipboard polling:

```sh
hcp --diagnostic
```

This will print all relevant environment variables, the detected display server, and warnings if your setup is not compatible with X11 clipboard polling.

### Other Development Instructions
- Make sure you have the required build dependencies:
  ```sh
  sudo apt-get update
  sudo apt-get install -y build-essential libx11-dev dpkg-dev
  ```
- To clean up previous builds, the script automatically removes any old build directories.
- After building, you can install the package with:
  ```sh
  sudo dpkg -i hcp_*.deb
  sudo apt-get install -f -y  # To fix any missing dependencies
  ```
- To test the clipboard manager, you can run:
  ```sh
  ./hcp service start &
  ./hcp list
  ./hcp pop
  ```
- For systemd integration, follow the post-install instructions printed by the script.

---

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

## Manual Page

After installing the package, you can view the manual page for usage and options:

```
man hcp
```

This provides detailed information about available commands and usage examples.
