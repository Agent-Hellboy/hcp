#include "../include/db.hpp"
#include <cassert>
#include <iostream>

// Test that clipboard history loads without crashing
void test_load_history() {
    try {
        auto history = load_clipboard_blocks();
        std::cout << "[PASS] test_load_history -> Loaded " << history.size() << " clipboard entries.\n";
    } catch (const std::exception& e) {
        std::cout << "[FAIL] test_load_history -> Exception: " << e.what() << "\n";
    } catch (...) {
        std::cout << "[ERROR] test_load_history -> Unknown error\n";
    }
}

// (Optional) Test that the GUI can be initialized (headless)
// This is a smoke test, not a full GUI test
#ifdef HAS_GTK
#include <gtk/gtk.h>
void test_gtk_init() {
    try {
        int argc = 0;
        char **argv = nullptr;
        gtk_init(&argc, &argv);
        std::cout << "[PASS] test_gtk_init -> GTK initialized successfully.\n";
    } catch (const std::exception& e) {
        std::cout << "[FAIL] test_gtk_init -> Exception: " << e.what() << "\n";
    } catch (...) {
        std::cout << "[ERROR] test_gtk_init -> Unknown error\n";
    }
}
#endif

int main() {
    test_load_history();
#ifdef HAS_GTK
    test_gtk_init();
#endif
    std::cout << "All tray app tests completed.\n";
    return 0;
}
