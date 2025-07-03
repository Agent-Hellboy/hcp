/*
This file will save history as 
binary blocks where each block contains:
- 4-byte length (uint32_t) of the clipboard content
- Variable-length clipboard content data

The format allows efficient appending of new entries
and reading of the entire history in reverse chronological order.

*/
#ifndef DB_HPP
#define DB_HPP
#include <vector>
#include <string>


extern const int HCP_MAX_HISTORY;
// Block store clipboard history
void append_clipboard_block(const std::string& entry);
std::vector<std::string> load_clipboard_blocks();

#endif // DB_HPP 