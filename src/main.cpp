#include "main.hpp"
#include "clipboard_mgmt.hpp"
#include "db.hpp"
#include "logging.hpp"
#include <X11/Xlib.h>
#include <algorithm>
#include <chrono>
#include <cstdlib>
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
  // Detect non-X11 session and warn
  const char *session_type = getenv("XDG_SESSION_TYPE");
  const char *display_env = getenv("DISPLAY");
  bool not_x11 = false;
  if (session_type) {
    std::string stype(session_type);
    if (stype != "x11")
      not_x11 = true;
  } else if (!display_env) {
    not_x11 = true;
  }
  if (not_x11) {
    // ANSI escape codes for bold red
    const char *bold = "\033[1m";
    const char *red = "\033[31m";
    const char *reset = "\033[0m";
    std::cerr << bold << red
              << "[hcp] WARNING: You're not running an X11 session. Clipboard "
                 "history polling is only supported under X11.\n"
              << reset;
    std::cerr << bold << red
              << "         Clipboard polling is NOT supported under Wayland or "
                 "other non-X11 display systems due to security restrictions."
              << reset << std::endl;
  }
  if (argc < 2 || std::string(argv[1]) == "--help" ||
      std::string(argv[1]) == "-h") {
    // ANSI escape codes for color
    const char *bold = "\033[1m";
    const char *green = "\033[32m";
    const char *cyan = "\033[36m";
    const char *reset = "\033[0m";
    std::cout << bold << green << "hcp - Historical Clipboard Manager for X11"
              << reset << std::endl;
    std::cout << cyan << "\nUsage:" << reset << std::endl;
    std::cout << "  hcp service start        " << cyan
              << "# Start clipboard monitoring service" << reset << std::endl;
    std::cout << "  hcp list                 " << cyan
              << "# List clipboard history" << reset << std::endl;
    std::cout << "  hcp <index>              " << cyan
              << "# Print clipboard entry at <index>" << reset << std::endl;
    std::cout << "  hcp pop                  " << cyan
              << "# Remove most recent clipboard entry" << reset << std::endl;
    std::cout << "  hcp --help | -h          " << cyan
              << "# Show this help message" << reset << std::endl;
    std::cout << "\n" << bold << "Description:" << reset << std::endl;
    std::cout
        << "  hcp is a lightweight clipboard manager for X11 systems. It "
           "captures clipboard entries,\n"
           "  maintains a history, and allows you to list, print, or remove "
           "entries.\n"
           "  Designed for reliability and minimalism, it works directly with "
           "the X11 clipboard\n"
           "  and is suitable for use as a background service or on-demand.\n"
        << std::endl;
    return 0;
  }
  std::string cmd = argv[1];
  if (cmd == "service" && argc > 2 && std::string(argv[2]) == "start") {
    run_service();
  } else if (cmd == "list") {
    list_history();
  } else if (cmd == "pop") {
    pop_clipboard_entry();
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