#include <sdbus-c++/sdbus-c++.h>
#include <vector>
#include <string>
#include <iostream>
#include <unistd.h>
#include "permission_manager.h"


int main(int argc, char *argv[])
{
    // Create proxy object for the concatenator object on the server side
    sdbus::ServiceName service_name{"com.system.permissions"};
    sdbus::ObjectPath object_path{"/"};
    auto concatenatorProxy = sdbus::createProxy(std::move(service_name), std::move(object_path));
    sdbus::InterfaceName interface_name{"com.system.permissions"};

    enum Permissions permision_code = SystemTime;

    // Invoke concatenate again, this time with no numbers and we shall get an error
    {
        try
        {
            concatenatorProxy->callMethod("RequestPermission").onInterface(interface_name).withArguments((int)permision_code);
            // concatenatorProxy->callMethod("RequestPermission").onInterface(interfaceName).withArguments(1)/*.dontExpectReply()*/;
        }
        catch(const sdbus::Error& e)
        {
            std::cerr << "Got concatenate error " << e.getName() << " with message " << e.getMessage() << std::endl;
        }

   sleep(1);

        try
        {
            bool test_bool = false;
            concatenatorProxy->callMethod("CheckApplicationHasPermission").onInterface(interface_name).withArguments("/home/sarwx/kernel/dbus_test/test_proj1/build/client", (int)permision_code).storeResultsTo(test_bool);
            std::cout << test_bool << std::endl;
        }
        catch(const sdbus::Error& e)
        {
            std::cerr << "Got concatenate error " << e.getName() << " with message " << e.getMessage() << std::endl;
        }        
    }

    // Give sufficient time to receive 'concatenated' signal from the first concatenate invocation
 

    return 0;
}
