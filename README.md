# hcp

- A simple clipboard manager for Linux that saves your clipboard history to a file, with each clipboard entry separated by a distinct marker.

- Save and list clipboard entries: if you've copied (Ctrl+C) text 8 times, you can access all 8 previous clipboard entries—similar to how you can use `git stash pop` to retrieve past stashes.

**Note:** This project currently targets POSIX systems only and uses APIs such as `mkdir`, `getenv`, `popen`, and `stat`. If there is significant user interest, I plan to migrate to cross-platform C++17 APIs in the future.

## Installation

Download the latest `.deb` package from [the Releases page](https://github.com/Agent-Hellboy/hcp/releases) and install it with:
```sh
sudo dpkg -i hcp_1.0.0.deb
```

## Usage

### Start the Clipboard Service
This will monitor your clipboard and save changes:
```sh
hcp service start &
```

or as a systemd service (Recommended):

```sh
sudo systemctl daemon-reload
sudo systemctl enable hcp.service
sudo systemctl start hcp.service
```

(You will see these instructions after the installation is finished.)

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
- The maximum number of entries is set by `HCP_MAX_HISTORY` (default: 10).

## Troubleshooting
- Make sure `xclip` is installed and working:
  ```sh
  xclip -selection clipboard -o
  ```
- If `~/.hcp/history.block` is not created, check `~/.hcp/service.log` for errors.

## License
See [LICENSE](LICENSE).
