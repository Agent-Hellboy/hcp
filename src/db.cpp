#include "db.hpp"
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <algorithm>

const int HCP_MAX_HISTORY = 10;
const std::string HCP_BLOCK_DB = std::string(getenv("HOME")) + "/.hcp/history.block";

// Append a clipboard entry to the block file
void append_clipboard_block(const std::string& entry) {
    std::string dir = std::string(getenv("HOME")) + "/.hcp";
    mkdir(dir.c_str(), 0700); // Ensure directory exists

    std::ofstream out(HCP_BLOCK_DB, std::ios::binary | std::ios::app);
    uint32_t len = entry.size();
    out.write(reinterpret_cast<const char*>(&len), sizeof(len));
    out.write(entry.data(), len);
}

// Read all clipboard entries from the block file (most recent first)
std::vector<std::string> load_clipboard_blocks() {
    std::vector<std::string> history;
    std::ifstream in(HCP_BLOCK_DB, std::ios::binary);
    while (in) {
        uint32_t len = 0;
        in.read(reinterpret_cast<char*>(&len), sizeof(len));
        if (!in || len == 0) break;
        std::string entry(len, '\0');
        in.read(&entry[0], len);
        if (!in) break;
        history.push_back(entry);
    }
    std::reverse(history.begin(), history.end()); // Most recent first
    return history;
} 