#include <sdbus-c++/sdbus-c++.h>
#include <vector>
#include <string>
#include <iostream>
#include <limits.h>   // for PATH_MAX
#include <unistd.h>
#include "permission_manager.h"

pid_t current_pid = 0;

// void RequestPermission(permissionEnumCode: int)
void RequestPermission(int permission_code)
{
    // Return error if permission code is not valid
    if (!is_valid_permission(permission_code))
        throw sdbus::Error(sdbus::Error::Name{"com.system.Permissions.Error"}, 
                                                "Not valid permission code");
    std::cout << current_pid << std::endl;

    // Try to get system path to the executable
    std::string proc_path = "/proc/" + std::to_string(current_pid) + "/exe";
    //TEST
    std::cout << proc_path << std::endl;

    char path[PATH_MAX];

    ssize_t len = readlink(proc_path.c_str(), path, sizeof(path) - 1);

    if (len == -1) {
        // В случае ошибки выбрасываем D-Bus ошибку
        throw sdbus::Error(sdbus::Error::Name{"com.system.Permissions.Error"}, 
                "Error reading symbolic link: " + std::string(strerror(errno)));
    } else {
        // Завершаем строку
        path[len] = '\0';
        //TEST
        std::cout << "Path of sender: " << path << std::endl;
    }
};

void HandleRequestPermission(sdbus::MethodCall call)
{
        // Сохраняем текущий PID
        current_pid = call.getCredsPid();

        // Достаем аргумент
        int permission_code;
        call >> permission_code;

        std::cout << std::endl << permission_code << std::endl;

        // Вызов основной функции
        RequestPermission(permission_code);

        // Пустой ответ
        auto reply = call.createReply();
        reply.send();
}

int main(int argc, char *argv[])
{
    // Create D-Bus connection to the session bus and requests a well-known name on it.
    sdbus::ServiceName serviceName{"com.system.permissions"};
    auto connection = sdbus::createSessionBusConnection(serviceName);       // Соединение на сессионной шине

    // Create concatenator D-Bus object.
    sdbus::ObjectPath objectPath{"/com/system/permissions"};
    auto concatenator = sdbus::createObject(*connection, std::move(objectPath));


    sdbus::InterfaceName interfaceName{"org.sdbuscpp.Concatenator"};
    // Register D-Bus methods and signals on the concatenator object, and exports the object.
    concatenator->addVTable(
        sdbus::MethodVTableItem{
            sdbus::MethodName{"RequestPermission"},  // Имя метода
            sdbus::Signature{"i"},  // Типы аргументов метода (в данном случае int, т.е. 'i')
            {},  // Параметры для метода (если не нужны, то оставляем пустыми)
            sdbus::Signature{},  // Тип возвращаемого значения (пусто, т.к. метод не возвращает значение)
            {},  // Параметры для возвращаемого значения (если не нужны, то оставляем пустыми)
            HandleRequestPermission,  // Обработчик метода
            {}
        }
    ).forInterface("com.system.Permissions");  // Интерфейс


    // concatenator->addVTable(sdbus::registerMethod("RequestPermission").implementedAs(HandleRequestPermission))
    //                        .forInterface("com.system.Permissions");

    // Run the loop on the connection.
    connection->enterEventLoop();
}
