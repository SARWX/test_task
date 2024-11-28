#include <sdbus-c++/sdbus-c++.h>
#include <vector>
#include <string>
#include <iostream>
#include <unistd.h>
#include "permission_manager.h"


int main(int argc, char *argv[])
{
    // Create proxy object for the concatenator object on the server side
    sdbus::ServiceName destination{"com.system.permissions"};
    sdbus::ObjectPath objectPath{"/com/system/permissions"};
    auto concatenatorProxy = sdbus::createProxy(std::move(destination), std::move(objectPath));
    sdbus::InterfaceName interfaceName{"com.system.Permissions"};

    enum Permissions permision_code = SystemTime;

    // Invoke concatenate again, this time with no numbers and we shall get an error
    {
        try
        {
            concatenatorProxy->callMethod("RequestPermission").onInterface(interfaceName).withArguments((int)permision_code);
            // concatenatorProxy->callMethod("RequestPermission").onInterface(interfaceName).withArguments(1)/*.dontExpectReply()*/;
        }
        catch(const sdbus::Error& e)
        {
            std::cerr << "Got concatenate error " << e.getName() << " with message " << e.getMessage() << std::endl;
        }
    }

    // Give sufficient time to receive 'concatenated' signal from the first concatenate invocation
    sleep(1);

    return 0;
}
