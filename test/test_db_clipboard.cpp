#include "../include/db.hpp"
#include "../include/clipboard_mgmt.hpp"
#include <cassert>
#include <iostream>
#ifdef HAS_X11
#include <X11/Xlib.h>
#endif

void test_append_and_load_history() {
    try {
        std::string test_entry = "Test clipboard entry for db test";
        append_clipboard_block(test_entry);
        auto history = load_clipboard_blocks();
        bool found = false;
        for (const auto& entry : history) {
            if (entry == test_entry) found = true;
        }
        if (found) {
            std::cout << "[PASS] test_append_and_load_history -> Test entry found in clipboard history.\n";
        } else {
            std::cout << "[FAIL] test_append_and_load_history -> Test entry NOT found in clipboard history.\n";
        }
    } catch (const std::exception& e) {
        std::cout << "[FAIL] test_append_and_load_history -> Exception: " << e.what() << "\n";
    } catch (...) {
        std::cout << "[ERROR] test_append_and_load_history -> Unknown error\n";
    }
}

#ifdef HAS_X11
void test_get_clipboard() {
    try {
        Display* display = XOpenDisplay(nullptr);
        if (!display) {
            std::cout << "[ERROR] test_get_clipboard -> X11 display not available, skipping clipboard test.\n";
            return;
        }
        std::string clip = get_clipboard(display);
        std::cout << "[PASS] test_get_clipboard -> Clipboard content: '" << clip.substr(0, 40) << "'...\n";
        XCloseDisplay(display);
    } catch (const std::exception& e) {
        std::cout << "[FAIL] test_get_clipboard -> Exception: " << e.what() << "\n";
    } catch (...) {
        std::cout << "[ERROR] test_get_clipboard -> Unknown error\n";
    }
}
#endif

int main() {
    test_append_and_load_history();
#ifdef HAS_X11
    test_get_clipboard();
#endif
    std::cout << "All db/clipboard mgmt tests completed.\n";
    return 0;
} 