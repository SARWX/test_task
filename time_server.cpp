#include <sdbus-c++/sdbus-c++.h>
#include <iostream>
#include <limits.h>
#include <unistd.h>
#include <string>
#include <stdexcept>
#include <ctime>
#include "permission_manager.h"


// Обработчик метода GetSystemTime
uint64_t GetSystemTime(sdbus::MethodCall call) {
    uint64_t timestamp = 0; /* Текущее время (результат) */

    pid_t current_pid = call.getCredsPid();         /* PID вызвавшего процесса */
    char path[PATH_MAX];                            /* Массив для получения абсолютного пути процесса */
    pid_to_path(current_pid, path);                 /* Получить абсолютный путь по PID */
    enum Permissions req_permision = SystemTime;    /* Требуемое разрешение */

    // Создать прокси для com.system.permissions
    sdbus::ServiceName service_name{"com.system.permissions"};
    sdbus::ObjectPath object_path{"/"};
    auto permission_proxy = sdbus::createProxy(std::move(service_name), std::move(object_path));
    sdbus::InterfaceName interface_name{"com.system.permissions"};
    
    bool have_permission = false;
    try
    {
        permission_proxy->callMethod("CheckApplicationHasPermission").onInterface(interface_name).withArguments(path, (int)req_permision).storeResultsTo(have_permission);
        // TEST
        std::cout << have_permission << std::endl;
    }
    catch(const sdbus::Error& e)
    {
        std::cerr << "Got concatenate error " << e.getName() << " with message " << e.getMessage() << std::endl;
    }    

    // Проверяем разрешение для получения времени
    if (!have_permission) 
    {
        // Создаём D-Bus ответ с ошибкой
        auto reply = call.createErrorReply(

        sdbus::Error(sdbus::Error::Name{"com.system.time.Error.UnauthorizedAccess"}, 
            "Unauthorized access: Application does not have permission to access system time.")
        // sdbus::Error(sdbus::Error::Name{"UnauthorizedAccess"},
        // "Unauthorized access: Application does not have permission to access system time.")
        );
        reply.send();  // Отправляем ошибку клиенту
        return timestamp;        // Завершаем выполнение метода


        // throw sdbus::Error(sdbus::Error::Name{"UnauthorizedAccess"},
        //     "Unauthorized access: Application does not have permission to access system time.");
    }
    else
    {
        // Получаем текущую метку времени (timestamp)
        // timestamp = (uint64_t)(std::time(nullptr));
        timestamp = (uint64_t)(std::time(nullptr));
        std::cout << timestamp << std::endl;
    }

    // Отправить ответ с timestamp
    auto reply = call.createReply();
    // reply << timestamp;
    // std::cout << reply. << std::endl;
    reply << (uint64_t)(timestamp);
    reply.send();
    // std::cout << "GGGGG" << std::endl;
    return timestamp;
}

int main() {
    // Создаем подключение к сессионной шине D-Bus
    sdbus::ServiceName service_name{"com.system.time"};
    auto connection = sdbus::createSessionBusConnection(service_name);
    sdbus::ObjectPath object_path{"/"};

    // Создаем объект
    auto time_object = sdbus::createObject(*connection, std::move(object_path));

    time_object->addVTable(
        sdbus::MethodVTableItem{
            sdbus::MethodName{"GetSystemTime"},  // Имя метода
            sdbus::Signature{""}, 
            {}, 
            sdbus::Signature{"t"},  // Тип возвращаемого значения - uint64_t
            {}, 
            &GetSystemTime,  // Обработчик метода
            {}
        }
    ).forInterface("com.system.time");

    // Создаем Прокси сервиса com.system.permissions
    sdbus::ServiceName proxy_name{"com.system.permissions"};
    sdbus::ObjectPath proxy_path{"/"};
    auto concatenatorProxy = sdbus::createProxy(std::move(proxy_name), std::move(proxy_path));
    sdbus::InterfaceName interfaceName{"com.system.permissions"};


    // Запуск основного цикла обработки событий D-Bus
    connection->enterEventLoop();
}
