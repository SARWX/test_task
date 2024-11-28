#include <sdbus-c++/sdbus-c++.h>
#include <vector>
#include <string>
#include <iostream>
#include <unistd.h>
#include <limits.h>   // for PATH_MAX
#include "permissions_db.h"   // for PATH_MAX


// Yeah, global variable is ugly, but this is just an example and we want to access
// the concatenator instance from within the concatenate method handler to be able
// to emit signals.
sdbus::IObject* g_concatenator{};

// Мой объект для работы с базой данных
permissions_db *g_db = nullptr;

void concatenate(sdbus::MethodCall call)
{
    // 1 - GET PATH OF SENDER
    // Try to detect sender
    pid_t pid = call.getCredsPid();
    // Print sender
    std::cout << std::endl << "Sender: " << call.getSender() << std::endl << "PID: " << pid << std::endl;

    // Try to get system path to the executable
    std::string proc_path = "/proc/" + std::to_string(pid) + "/exe";
    std::cout << proc_path << std::endl;

    char path[PATH_MAX];

    ssize_t len = readlink(proc_path.c_str(), path, sizeof(path) - 1);
    
    if (len == -1) {
        // В случае ошибки выводим информацию об ошибке
        std::cerr << "Error reading symbolic link: " << strerror(errno) << std::endl;
    } else {
        // Завершаем строку
        path[len] = '\0';
        std::cout << "Path of sender: " << path << std::endl;
    }

    // 2 - INSERT PATH TO DATA BASE
    g_db->insert_permission(path, 0);




    
    // Deserialize the collection of numbers from the message
    std::vector<int> numbers;
    call >> numbers;

    // Deserialize separator from the message
    std::string separator;
    call >> separator;

    // Return error if there are no numbers in the collection
    if (numbers.empty())
        throw sdbus::Error(sdbus::Error::Name{"org.sdbuscpp.Concatenator.Error"}, "No numbers provided");

    std::string result;
    for (auto number : numbers)
    {
        result += (result.empty() ? std::string() : separator) + std::to_string(number);
    }

    // Serialize resulting string to the reply and send the reply to the caller
    auto reply = call.createReply();
    reply << result;
    reply.send();

    // Emit 'concatenated' signal
    sdbus::InterfaceName interfaceName{"org.sdbuscpp.Concatenator"};
    sdbus::SignalName signalName{"concatenated"};
    auto signal = g_concatenator->createSignal(interfaceName, signalName);
    signal << result;
    g_concatenator->emitSignal(signal);
}

int main(int argc, char *argv[])
{
    // Create D-Bus connection to (either the session or system) bus and requests a well-known name on it.
    sdbus::ServiceName serviceName{"com.system.permissions"};
    auto connection = sdbus::createSessionBusConnection(serviceName);

    // Create concatenator D-Bus object.
    sdbus::ObjectPath objectPath{"/org/sdbuscpp/concatenator"};
    auto concatenator = sdbus::createObject(*connection, std::move(objectPath));

    g_concatenator = concatenator.get();

    // Register D-Bus methods and signals on the concatenator object, and exports the object.
    sdbus::InterfaceName interfaceName{"org.sdbuscpp.Concatenator"};
    concatenator->addVTable( sdbus::MethodVTableItem{sdbus::MethodName{"concatenate"}, sdbus::Signature{"ais"}, {}, sdbus::Signature{"s"}, {}, &concatenate, {}}
                           , sdbus::SignalVTableItem{sdbus::MethodName{"concatenated"}, sdbus::Signature{"s"}, {}, {}} )
                           .forInterface(interfaceName);

    // Создаем экземпляр permissions_db
    permissions_db db("permissions.db");
    // Присваиваем его глобальной переменной
    g_db = &db;

    // Run the I/O event loop on the bus connection.
    connection->enterEventLoop();
}
