#include "permission_manager.h"
#include <iostream>
#include <sdbus-c++/sdbus-c++.h>
#include <string>

bool is_valid_permission(int value) {
  return value >= 0 && value < NotValidPermission;
}

void pid_to_path(pid_t pid, char *path, size_t size) {
  // convert PID to path in procfs
  std::string proc_path = "/proc/" + std::to_string(pid) + "/exe";
#ifdef DEBUG
  std::cout << proc_path << std::endl;
#endif
  // convert symbol link to its value
  ssize_t len = readlink(proc_path.c_str(), path, size - 1);

  if (len == -1) {
    // in case of error, throw D-Bus Error
    throw sdbus::Error(sdbus::Error::Name{"com.system.Permissions.Error"},
                       "Error reading symbolic link: " +
                           std::string(strerror(errno)));
  } else {
    // complet c-string
    path[len] = '\0';
#ifdef DEBUG
    std::cout << "System path: " << path << std::endl;
#endif
  }
}
