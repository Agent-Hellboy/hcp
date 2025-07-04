#include "main.hpp"
#include "clipboard_mgmt.hpp"
#include "db.hpp"
#include "logging.hpp"
#include <X11/Xlib.h>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>

void run_service() {
  log_event("[hcp] Service started.");
  Display *display = XOpenDisplay(nullptr);
  if (!display) {
    log_event("[hcp] ERROR: Cannot open X11 display");
    return;
  }
  std::string last_clip;
  auto history = load_clipboard_blocks();
  if (!history.empty())
    last_clip = history.front();
  while (true) {
    std::string current = get_clipboard(display);
    if (!current.empty() && current != last_clip) {
      append_clipboard_block(current);
      last_clip = current;
      log_event("[hcp] New clipboard entry saved. Size: " +
                std::to_string(current.size()));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }
  XCloseDisplay(display);
}

void list_history() {
  auto history = load_clipboard_blocks();
  int idx = 1;
  for (const auto &entry : history) {
    std::cout << idx << ": "
              << (entry.size() > 40 ? entry.substr(0, 40) + "..." : entry)
              << std::endl;
    ++idx;
  }
}

void print_entry(int index) {
  auto history = load_clipboard_blocks();
  if (index < 1 || index > (int)history.size()) {
    std::cerr << "Invalid index." << std::endl;
    return;
  }
  std::cout << history[index - 1] << std::endl;
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: hcp <command> [args]\nCommands: service start | list "
                 "| <index>"
              << std::endl;
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