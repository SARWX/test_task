#include <sdbus-c++/sdbus-c++.h>
#include <vector>
#include <string>
#include <iostream>
#include "permission_manager.h"

// void RequestPermission(permissionEnumCode: int)
void RequestPermission(int permission_code)
    {
        // Return error if permission code is not valid
        if (!is_valid_permission(permission_code))
            throw sdbus::Error(sdbus::Error::Name{"com.system.Permissions.Error"}, "Not valid permission code");

    };

int main(int argc, char *argv[])
{
    // Create D-Bus connection to the session bus and requests a well-known name on it.
    sdbus::ServiceName serviceName{"com.system.permissions"};
    auto connection = sdbus::createSessionBusConnection(serviceName);       // Соединение на сессионной шине

    // Create concatenator D-Bus object.
    sdbus::ObjectPath objectPath{"/com/system/permissions"};
    auto concatenator = sdbus::createObject(*connection, std::move(objectPath));

    // Register D-Bus methods and signals on the concatenator object, and exports the object.
    concatenator->addVTable(sdbus::registerMethod("RequestPermission").implementedAs(RequestPermission))
                           .forInterface("com.system.Permissions");

    // Run the loop on the connection.
    connection->enterEventLoop();
}
