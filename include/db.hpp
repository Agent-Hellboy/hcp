/*
This file will save history as
binary blocks where each block contains:
- 4-byte length (uint32_t) of the clipboard content
- Variable-length clipboard content data

[4 bytes length][N bytes clipboard text]

The format allows efficient appending of new entries
and reading of the entire history in reverse chronological order.

*/
#ifndef DB_HPP
#define DB_HPP
#include <string>
#include <vector>
#include <cstdint>

extern const int HCP_MAX_HISTORY;
constexpr uint32_t MAX_CLIPBOARD_SIZE = 16 * 1024 * 1024; // 16MB typical max

// Block store clipboard history
void append_clipboard_block(const std::string &entry);
std::vector<std::string> load_clipboard_blocks();

std::string get_hcp_dir();
std::string get_hcp_block_db();

#endif // DB_HPP