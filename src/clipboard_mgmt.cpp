/*
Clipboard reading process (robust X11 approach):

1. Ask: "Who owns the clipboard?"
2. Request data from the owner using XConvertSelection.
3. Create a tiny, invisible X11 window to receive the data.
   - If window creation fails, abort.
4. Wait for a SelectionNotify event using select() with a timeout (1s):
   - If the event arrives, retrieve the clipboard data.
   - If the timeout occurs (clipboard owner unresponsive), clean up and abort.
5. Retrieve the data from the window property and clean up.

Clipboard Copy/Paste Flow in X11:

   +-------------+
   |   Firefox   |   (Clipboard Owner)
   +------+------+
          |
 1. Ctrl+C|
 ---------+
          |
          | 2. XSetSelectionOwner("CLIPBOARD")
          v
   +------+------+
   |  X11 Server |
   +------+------+
          |
          |
 3. No data stored — just remembers owner
          |
          |
 4. Ctrl+V|
 <--------+
          |
   +------+------+
   |   Terminal  |   (Requester)
   +------+------+
          |
          |
 5. Terminal asks: "Who owns CLIPBOARD?"
          |
          v
   +------+------+
   |  X11 Server |
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

---------------------------------------------------------

+------------------------+
| Start clipboard request|   ← XConvertSelection()
+-----------+------------+
            |
            v
+--------------------------+
| Get X11 file descriptor  | ← ConnectionNumber(display)
+--------------------------+
            |
            v
+-----------------------------+
| Set 1s timeout using timeval|
+-----------------------------+
            |
            v
+-------------------------------+
| Use select() to wait for data |
| on X11 FD or timeout          |
+-------------------------------+
     |                         |
     |                         |
     | (data available)        | (timeout)
     v                         v
+---------------------+   +--------------------------+
| Check XPending()    |   | Timeout occurred         |
| → any event in queue?|   | - Destroy dummy window   |
+---------------------+   | - Return empty string    |
     |                   +--------------------------+
     |
     | (yes)
     v
+--------------------------+
| Get next X event         | ← XNextEvent()
+--------------------------+
     |
     | (is it SelectionNotify?)
     |
     +-- yes --> Done! Clipboard data is ready
     |
     +-- no --> Loop again until timeout or match

Explanation of the select() Event Loop Flow Diagram:

1. Start clipboard request: The program initiates a clipboard data request using
XConvertSelection().
2. Get X11 file descriptor: It retrieves the file descriptor for the X11
connection using ConnectionNumber(display).
3. Set timeout: A timeout (e.g., 1 second) is set using a timeval struct.
4. Wait for data or timeout: The program uses select() to wait for data (an
event) on the X11 file descriptor, or for the timeout to occur.
5. Two possible outcomes:
   - Data available:
     - If data is available, check with XPending() if there are any X events in
the queue.
     - If yes, use XNextEvent() to get the next event.
     - If the event is SelectionNotify, the clipboard data is ready and the
process is done.
     - If not, the loop continues until the timeout or a matching event is
found.
   - Timeout:
     - If no data arrives within the timeout, the program destroys the dummy
window and returns an empty string, indicating failure.

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
  if (window == None) {
    return "";
  }

  XConvertSelection(display, clipboard, target, target, window, CurrentTime);
  XFlush(display);

  XEvent event;
  struct timeval timeout;
  fd_set readfds;
  int x11_fd = ConnectionNumber(display);

  while (true) {
    FD_ZERO(&readfds);
    FD_SET(x11_fd, &readfds);
    timeout.tv_sec = 1; // 1 second timeout
    timeout.tv_usec = 0;

    if (select(x11_fd + 1, &readfds, NULL, NULL, &timeout) > 0) {
      if (XPending(display)) {
        XNextEvent(display, &event);
        if (event.type == SelectionNotify)
          break;
      }
    } else {
      // Timeout occurred
      XDestroyWindow(display, window);
      return "";
    }
  }

  Atom actual_type;
  int actual_format;
  unsigned long nitems, bytes_after;
  unsigned char *prop = nullptr;

  // Limit clipboard data to reasonable size (e.g., 1MB)
  const long max_clipboard_size = 1024 * 1024;
  int result = XGetWindowProperty(
      display, window, target, 0, max_clipboard_size, False, AnyPropertyType,
      &actual_type, &actual_format, &nitems, &bytes_after, &prop);

  std::string clipboard_content;
  if (result == Success && prop) {
    clipboard_content.assign(reinterpret_cast<char *>(prop), nitems);
    XFree(prop);
  }
  XDestroyWindow(display, window);
  return clipboard_content;
}