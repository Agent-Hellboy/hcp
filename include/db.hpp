#ifndef DB_HPP
#define DB_HPP
#include <vector>
#include <string>

extern const int HCP_MAX_HISTORY;
// Loads clipboard history, one entry per line
std::vector<std::string> load_latest_clipboard();
// Saves clipboard history, one entry per line
void save_clipboard(const std::vector<std::string>& history);
#endif // DB_HPP 