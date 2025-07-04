#ifndef CLIPBOARD_MGMT_HPP
#define CLIPBOARD_MGMT_HPP
#include <X11/Xlib.h>
#include <string>

std::string get_clipboard(Display *display);
#endif // CLIPBOARD_MGMT_HPP