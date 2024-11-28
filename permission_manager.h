#ifndef PERMISSION_MANAGER_H
#define PERMISSION_MANAGER_H

enum Permissions {
    SystemTime = 0,         // Разрешение на доступ к системному времени
    // ...
    NotValidPermission      // Конец списка перечеслений
};

bool is_valid_permission(int value) {
    return value >= 0 && value < NotValidPermission;  // Проверка в диапазоне
}

#endif // PERMISSION_MANAGER_H
