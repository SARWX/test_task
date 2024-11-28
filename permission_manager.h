#ifndef PERMISSION_MANAGER_H
#define PERMISSION_MANAGER_H

#include <unistd.h>

enum Permissions {
  SystemTime = 0, /* permission to get system time */
  // ...
  NotValidPermission /* end of enum Permissions */
};

bool is_valid_permission(int value);

void pid_to_path(pid_t pid, char *path, size_t size);

#endif // PERMISSION_MANAGER_H
