#include "permission_manager.h"
#include <iostream>
#include <sdbus-c++/sdbus-c++.h>
#include <string>

/**
 * @brief Checks if the given permission value is valid.
 * 
 * This function checks whether the provided permission value is within
 * the valid range of permission codes.
 * 
 * @param value The permission code to check.
 * @retval  true If the permission code is valid.
 *          false If the permission code is invalid.
 */
bool is_valid_permission(int value) {
  return value >= 0 && value < NotValidPermission;
}

/**
 * @brief Converts a process ID (PID) to the corresponding system path.
 * 
 * This function reads the symbolic link to the executable path of the process
 * with the given PID from the `/proc` filesystem.
 * 
 * @param pid The process ID to resolve.
 * @param path A character array where the resolved path will be stored.
 * @param size The size of the `path` buffer.
 * 
 * @throws sdbus::Error If there is an error reading the symbolic link.
 * 
 * @retval None.
 */
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
