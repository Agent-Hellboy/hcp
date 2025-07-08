#ifndef CLIPBOARD_MGMT_HPP
#define CLIPBOARD_MGMT_HPP
#include <X11/Xlib.h>
#include <string>

std::string get_clipboard(Display *display);
void set_clipboard(Display *display, const std::string &text);
#endif // CLIPBOARD_MGMT_HPP