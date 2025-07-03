#include "logging.hpp"
#include <cstdlib>
#include <fstream>
#include <sys/stat.h>

const std::string HCP_LOG_DIR = std::string(getenv("HOME")) + "/.hcp";
const std::string HCP_LOG_FILE = HCP_LOG_DIR + "/service.log";

void log_event(const std::string &msg) {
  struct stat st = {0};
  if (stat(HCP_LOG_DIR.c_str(), &st) == -1) {
    mkdir(HCP_LOG_DIR.c_str(), 0700);
  }
  std::ofstream log(HCP_LOG_FILE, std::ios::app);
  if (log.is_open()) {
    log << msg << std::endl;
  }
}