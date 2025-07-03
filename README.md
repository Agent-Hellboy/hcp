# hcp

- A simple clipboard manager for Linux that saves your clipboard history to a file, with each clipboard entry separated by a distinct marker.

- Save and list clipboard entries: if you've copied (Ctrl+C) text 8 times, you can access all 8 previous clipboard entries, similar to how you can use `git stash pop` to retrieve past stashes.

## Installation

Download the latest `.deb` package from [the Releases page](https://github.com/Agent-Hellboy/hcp/releases) and install it with:
```sh
sudo dpkg -i hcp_1.0.0.deb
```
follow instructions to start the hpc service and use binary to list the clipboards and print the nth entry

## Usage

### List Clipboard History
```sh
./hcp list
```

### Print a Specific Clipboard Entry
Print the Nth entry (as shown in the list):
```sh
./hcp <index>
# Example:
./hcp 2
```

## How It Works
- Clipboard entries are saved in `~/.hcp/db`.
- Each entry is separated by a line containing only:
  ```
  ---hcp-entry---
  ```
- The service logs events to `~/.hcp/service.log`.
- The maximum number of entries is set by `HCP_MAX_HISTORY` (default: 10).

## Troubleshooting
- Make sure `xclip` is installed and working:
  ```sh
  xclip -selection clipboard -o
  ```
- If `~/.hcp/db` is not created, check `~/.hcp/service.log` for errors.

## License
See [LICENSE](LICENSE).
