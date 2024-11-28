#include <sdbus-c++/sdbus-c++.h>
#include <vector>
#include <string>

int main(int argc, char *argv[])
{
    // Create D-Bus connection to the (either system or session) bus and requests a well-known name on it.
    sdbus::ServiceName serviceName{"org.sdbuscpp.concatenator"};
    auto connection = sdbus::createBusConnection(serviceName);

    // Create concatenator D-Bus object.
    sdbus::ObjectPath objectPath{"/org/sdbuscpp/concatenator"};
    auto concatenator = sdbus::createObject(*connection, std::move(objectPath));

    auto concatenate = [&concatenator](const std::vector<int> numbers, const std::string& separator)
    {
        // Return error if there are no numbers in the collection
        if (numbers.empty())
            throw sdbus::Error(sdbus::Error::Name{"org.sdbuscpp.Concatenator.Error"}, "No numbers provided");

        std::string result;
        for (auto number : numbers)
        {
            result += (result.empty() ? std::string() : separator) + std::to_string(number);
        }

        // Emit 'concatenated' signal
        concatenator->emitSignal("concatenated").onInterface("org.sdbuscpp.Concatenator").withArguments(result);

        return result;
    };

    // Register D-Bus methods and signals on the concatenator object, and exports the object.
    concatenator->addVTable(sdbus::registerMethod("concatenate").implementedAs(std::move(concatenate)),
                            sdbus::registerSignal("concatenated").withParameters<std::string>())
                           .forInterface("org.sdbuscpp.Concatenator");

    // Run the loop on the connection.
    connection->enterEventLoop();
}
