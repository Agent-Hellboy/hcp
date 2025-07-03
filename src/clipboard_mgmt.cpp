#include "clipboard_mgmt.hpp"
#include <cstdio>
#include <string>

std::string get_clipboard() {
  std::string result;
  FILE *pipe = popen("xclip -selection clipboard -o", "r");
  if (!pipe)
    return "";
  char buffer[128];
  while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
    result += buffer;
  }
  pclose(pipe);
  return result;
}