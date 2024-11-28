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

void pid_to_path(pid_t pid, char *path)
{
    // 2 - преобразуем pid к абсолютному пути в procfs
    std::string proc_path = "/proc/" + std::to_string(pid) + "/exe";
    //TEST
    std::cout << proc_path << std::endl;

    // 3 - преобразуем symbol link в системный путь к исполнямому файлу
    ssize_t len = readlink(proc_path.c_str(), path, sizeof(path) - 1);

    if (len == -1) {
        // В случае ошибки выбрасываем D-Bus ошибку
        throw sdbus::Error(sdbus::Error::Name{"com.system.Permissions.Error"}, 
                "Error reading symbolic link: " + std::string(strerror(errno)));
    } else {
        // Завершаем строку
        path[len] = '\0';
    }
    return;
}

#endif // PERMISSION_MANAGER_H
