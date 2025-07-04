/*
To read clipboard, you must:

    Ask: "Who owns it?"

    Request data from that owner.

    Wait for the reply.

    Retrieve the data.

flow
               +-------------+
               |   Firefox   |
               | (Clipboard  |
               |   Owner)    |
               +------+------+
                      |
         1. Ctrl+C    |
         -------------+
                      |
                      | 2. XSetSelectionOwner("CLIPBOARD")
                      v
               +------+------+
               |   X11 Server |
               +------+------+
                      |
                      |
      3. No data stored — just remembers owner
                      |
                      |
         4. Ctrl+V    |
         <------------+
                      |
               +------+------+
               |   Terminal   |
               | (Requester)  |
               +------+------+
                      |
                      |
         5. Terminal asks: "Who owns CLIPBOARD?"
                      |
                      v
               +------+------+
               |   X11 Server |
               +------+------+
                      |
         6. Server replies: "Firefox owns it"
                      |
                      v
               +-------------+
               |   Firefox   |
               +-------------+
                      |
      7. Firefox receives SelectionRequest
                      |
      8. Firefox replies with clipboard data
                      |
                      v
               +-------------+
               |   Terminal  |
               +-------------+
         9. Terminal receives & pastes data


*/
#include "clipboard_mgmt.hpp"
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <cstring>
#include <string>

// NOTE: This code was generated with ChatGPT. I understand the general flow,
// but haven't checked all X11 API details.
std::string get_clipboard(Display *display) {
  Atom clipboard = XInternAtom(display, "CLIPBOARD", False);
  Atom utf8 = XInternAtom(display, "UTF8_STRING", False);
  Atom target = utf8;

  Window owner = XGetSelectionOwner(display, clipboard);
  if (owner == None) {
    return "";
  }

  Window window = XCreateSimpleWindow(display, DefaultRootWindow(display), 0, 0,
                                      1, 1, 0, 0, 0);

  XConvertSelection(display, clipboard, target, target, window, CurrentTime);
  XFlush(display);

  XEvent event;
  while (true) {
    XNextEvent(display, &event);
    if (event.type == SelectionNotify)
      break;
  }

  Atom actual_type;
  int actual_format;
  unsigned long nitems, bytes_after;
  unsigned char *prop = nullptr;

  int result = XGetWindowProperty(display, window, target, 0, (~0L), False,
                                  AnyPropertyType, &actual_type, &actual_format,
                                  &nitems, &bytes_after, &prop);

  std::string clipboard_content;
  if (result == Success && prop) {
    clipboard_content.assign(reinterpret_cast<char *>(prop), nitems);
    XFree(prop);
  }
  XDestroyWindow(display, window);
  return clipboard_content;
}