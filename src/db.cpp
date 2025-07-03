#include "db.hpp"
#include "logging.hpp"
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <cstdint>

const int HCP_MAX_HISTORY = 20;
constexpr uint32_t MAX_CLIPBOARD_SIZE = 16 * 1024 * 1024; // 16MB typical max

std::string get_hcp_dir() {
  const char *home = getenv("HOME");
  if (!home)
    throw std::runtime_error("Home directory not set");
  return std::string(home) + "/.hcp";
}

std::string get_hcp_block_db() { return get_hcp_dir() + "/history.block"; }

// Append a clipboard entry to the block file
void append_clipboard_block(const std::string &entry) {
  std::string dir = get_hcp_dir();
  mkdir(dir.c_str(), 0700);

  std::ofstream out(get_hcp_block_db(), std::ios::binary | std::ios::app);
  if (!out.is_open()) {
    log_event(
        "[hcp] ERROR: Failed to open clipboard history block file for writing");
    return;
  }
  uint32_t len = entry.size();
  out.write(reinterpret_cast<const char *>(&len), sizeof(len));
  // std::ofstream does not throw exceptions by default, we have to check
  // manually using good
  if (!out.good()) {
    log_event("[hcp] ERROR: Failed to write clipboard entry data to history "
              "block file (possibly corrupted or truncated file)");
    return;
  }
  out.write(entry.data(), len);
  if (!out.good()) {
    log_event("[hcp] ERROR: Failed to write clipboard entry data to history "
              "block file (possibly corrupted or truncated file)");
    return;
  }
}

// Read all clipboard entries from the block file (most recent first)
std::vector<std::string> load_clipboard_blocks() {
  std::vector<std::string> history;
  std::ifstream in(get_hcp_block_db(), std::ios::binary);
  if (!in.is_open()) {
    log_event(
        "[hcp] ERROR: Failed to open clipboard history block file for reading");
    return history;
  }
  while (in) {
    uint32_t len = 0;
    in.read(reinterpret_cast<char *>(&len), sizeof(len));
    if (!in) {
      log_event("[hcp] ERROR: Failed to read length from clipboard history "
                "block file (possibly corrupted or truncated file)");
      break;
    }
    if (len == 0) {
      log_event("[hcp] WARNING: Encountered clipboard entry with zero length "
                "in history block file (skipping)");
      break;
    }
    if (len > MAX_CLIPBOARD_SIZE) {
      log_event("[hcp] WARNING: Clipboard entry size exceeds max allowed (" +
                std::to_string(len) +
                " bytes), skipping (possible corruption)");
      break;
    }
    std::string entry(len, '\0');
    in.read(&entry[0], len);
    if (!in) {
      log_event("[hcp] ERROR: Failed to read clipboard entry data from history "
                "block file (possibly corrupted or truncated file)");
      break;
    }
    history.push_back(entry);
  }
  std::reverse(history.begin(), history.end()); // Most recent first
  return history;
}