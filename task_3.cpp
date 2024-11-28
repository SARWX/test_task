#include <sdbus-c++/sdbus-c++.h>
#include <vector>
#include <string>
#include <iostream>
#include <unistd.h>
#include "permission_manager.h"


int main(int argc, char *argv[])
{
    // Create proxy object for time server
    sdbus::ServiceName ts_name{"com.system.time"};
    sdbus::ObjectPath ts_path{"/"};
    auto ts_proxy = sdbus::createProxy(std::move(ts_name), std::move(ts_path));
    sdbus::InterfaceName ts_interface{"com.system.time"};

    // Create proxy object for permission server
    sdbus::ServiceName ps_name{"com.system.permissions"};
    sdbus::ObjectPath ps_path{"/"};
    auto ps_proxy = sdbus::createProxy(std::move(ps_name), std::move(ps_path));
    sdbus::InterfaceName ps_interface{"com.system.permissions"};

    enum Permissions time_permision = SystemTime;
    int64_t timestamp = 0;
    std::string error_name;

    try
    {
        ts_proxy->callMethod("GetSystemTime").onInterface(ts_interface).storeResultsTo(timestamp);
        // ts_proxy->callMethod("GetSystemTime").onInterface(ts_interface);
        // ts_proxy.GetSystemTime();
        std::cout << "HELLO" << std::endl;
    }
    catch(const sdbus::Error& e)
    {
        error_name = e.getName();
        std::cerr << "GoSSSSt concatenate error " << error_name << " with message " << e.getMessage() << std::endl;
    }
    if((timestamp == 0) && (error_name == "com.system.time.Error.UnauthorizedAccess"))
    {
        try
        {
            std::cout << "1111" << std::endl;
            ps_proxy->callMethod("RequestPermission").onInterface(ps_interface).withArguments((int)time_permision);
            std::cout << "2222" << std::endl;
            ts_proxy->callMethod("GetSystemTime").onInterface(ts_interface).storeResultsTo(timestamp);
            std::cout << "3333" << std::endl;
            std::cout << "Current time: " << std::ctime((time_t *)&timestamp);
        }
        catch(const sdbus::Error& e)
        {
            std::cerr << "Got concatenate error " << error_name << " with message " << e.getMessage() << std::endl;
        }
    }

    sleep(1);
    return 0;
}
