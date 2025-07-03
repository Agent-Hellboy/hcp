# hcp

A simple clipboard manager for Linux that saves your clipboard history to a file, with each clipboard entry separated by a distinct marker.

## Features
- Automatically saves every clipboard change (no length restriction)
- Stores clipboard history in `~/.hcp/db`, with entries separated by `---hcp-entry---`
- Command-line interface to list and print clipboard history
- Simple LRU logic to avoid duplicates and limit history size
- Logs events to `~/.hcp/service.log`

## Installation

### Prerequisites
- Linux system
- `g++` (or another C++ compiler)
- `xclip` (for clipboard access)

### Build
```sh
make
```
This will build the `hcp` executable in the project directory.

## Usage

### Start the Clipboard Service
This will monitor your clipboard and save changes:
```sh
./hcp service start
```

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