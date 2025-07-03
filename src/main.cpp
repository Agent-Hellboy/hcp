#include "main.hpp"
#include "clipboard_mgmt.hpp"
#include "db.hpp"
#include "logging.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <algorithm>
#include <string>

void run_service() {
    log_event("[hcp] Service started.");
    std::vector<std::string> history = load_latest_clipboard();
    std::string last_clip = history.empty() ? "" : history.front();
    while (true) {
        std::string current = get_clipboard();
        if (!current.empty() && current != last_clip) {
            // Remove if already in history (LRU)
            history.erase(std::remove(history.begin(), history.end(), current), history.end());
            // Insert at front
            history.insert(history.begin(), current);
            // Trim to max size
            if (history.size() > HCP_MAX_HISTORY) {
                history.resize(HCP_MAX_HISTORY);
            }
            save_clipboard(history);
            last_clip = current;
            std::string logmsg = "[hcp] New clipboard entry saved. Size: " + std::to_string(current.size());
            log_event(logmsg);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void list_history() {
    auto history = load_latest_clipboard();
    int idx = 1;
    for (const auto& entry : history) {
        std::cout << idx << ": " << (entry.size() > 40 ? entry.substr(0, 40) + "..." : entry) << std::endl;
        ++idx;
    }
}

void print_entry(int index) {
    auto history = load_latest_clipboard();
    if (index < 1 || index > (int)history.size()) {
        std::cerr << "Invalid index." << std::endl;
        return;
    }
    std::cout << history[index - 1] << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: hcp <command> [args]\nCommands: service start | list | <index>" << std::endl;
        return 1;
    }
    std::string cmd = argv[1];
    if (cmd == "service" && argc > 2 && std::string(argv[2]) == "start") {
        run_service();
    } else if (cmd == "list") {
        list_history();
    } else {
        try {
            int idx = std::stoi(cmd);
            print_entry(idx);
        } catch (...) {
            std::cerr << "Unknown command or invalid index." << std::endl;
            return 1;
        }
    }
    return 0;
} 