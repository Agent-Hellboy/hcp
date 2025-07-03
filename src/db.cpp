#include "db.hpp"
#include <fstream>
#include <cstdlib>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

const int HCP_MAX_HISTORY = 10;
const std::string HCP_DB = std::string(getenv("HOME")) + "/.hcp/db";
const std::string HCP_SEPARATOR = "---hcp-entry---";

std::vector<std::string> load_latest_clipboard() {
    std::vector<std::string> history;
    std::ifstream infile(HCP_DB);
    if (!infile.is_open()) return history;
    std::string entry, line;
    while (std::getline(infile, line)) {
        if (line == HCP_SEPARATOR) {
            history.push_back(entry);
            entry.clear();
        } else {
            if (!entry.empty()) entry += "\n";
            entry += line;
        }
    }
    if (!entry.empty()) history.push_back(entry);
    return history;
}

void save_clipboard(const std::vector<std::string>& history) {
    std::string dir = std::string(getenv("HOME")) + "/.hcp";
    mkdir(dir.c_str(), 0700); 
    std::ofstream outfile(HCP_DB, std::ios::trunc);
    for (const auto& entry : history) {
        outfile << entry << std::endl << HCP_SEPARATOR << std::endl;
    }
} 