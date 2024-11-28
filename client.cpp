#include <sdbus-c++/sdbus-c++.h>
#include <string>
#include <vector>
#include <iostream>
#include <unistd.h>

void onConcatenated(sdbus::Signal signal)
{
    std::string concatenatedString;
    signal >> concatenatedString;

    std::cout << "Received signal with concatenated string " << concatenatedString << std::endl;
}

int main(int argc, char *argv[])
{
    // Create proxy object for the concatenator object on the server side. Since here
    // we are creating the proxy instance without passing connection to it, the proxy
    // will create its own connection automatically (to either system bus or session bus,
    // depending on the context).
    sdbus::ServiceName destination{"com.system.permissions"};
    sdbus::ObjectPath objectPath{"/org/sdbuscpp/concatenator"};
    auto concatenatorProxy = sdbus::createProxy(std::move(destination), std::move(objectPath));

    // Let's subscribe for the 'concatenated' signals
    sdbus::InterfaceName interfaceName{"org.sdbuscpp.Concatenator"};
    sdbus::SignalName signalName{"concatenated"};
    concatenatorProxy->registerSignalHandler(interfaceName, signalName, &onConcatenated);

    std::vector<int> numbers = {1, 2, 3};
    std::string separator = ":";

    sdbus::MethodName concatenate{"concatenate"};
    // Invoke concatenate on given interface of the object
    {
        auto method = concatenatorProxy->createMethodCall(interfaceName, concatenate);
        method << numbers << separator;
        try
        {
            auto reply = concatenatorProxy->callMethod(method);
            std::string result;
            reply >> result;
            assert(result == "1:2:3");
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    }

    // Invoke concatenate again, this time with no numbers and we shall get an error
    {
        auto method = concatenatorProxy->createMethodCall(interfaceName, concatenate);
        method << std::vector<int>() << separator;
        try
        {
            auto reply = concatenatorProxy->callMethod(method);
            assert(false);
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