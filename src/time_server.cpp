#include "permission_manager.h"
#include <ctime>
#include <iostream>
#include <limits.h>
#include <sdbus-c++/sdbus-c++.h>
#include <stdexcept>
#include <string>
#include <unistd.h>

uint64_t GetSystemTime(sdbus::MethodCall call) {
  uint64_t timestamp = 0; /* variable for result (current time) */

  pid_t current_pid = call.getCredsPid();      /* PID of calling process */
  char path[PATH_MAX];                         /* array for absolute path */
  enum Permissions req_permision = SystemTime; /* required permission */
  pid_to_path(current_pid, path, PATH_MAX);

  // create proxy of com.system.permissions
  sdbus::ServiceName ps_proxy_name{"com.system.permissions"};
  sdbus::ObjectPath ps_proxy_path{"/"};
  auto ps_proxy =
      sdbus::createProxy(std::move(ps_proxy_name), std::move(ps_proxy_path));
  sdbus::InterfaceName ts_interface{"com.system.permissions"};

  bool have_permission = false; /* variable for check result */
  try {
    ps_proxy->callMethod("CheckApplicationHasPermission")
        .onInterface(ts_interface)
        .withArguments(path, (int)req_permision)
        .storeResultsTo(have_permission);
#ifdef DEBUG
    std::cout << path << (have_permission ? "" : " doesn't")
              << " have permission: " << req_permision << std::endl;
#endif
  } catch (const sdbus::Error &e) {
    std::cerr << "Got concatenate error " << e.getName() << " with message "
              << e.getMessage() << std::endl;
  }

  if (!have_permission) {
    // return D-Bus reply with Error
    auto reply = call.createErrorReply(sdbus::Error(
        sdbus::Error::Name{"com.system.time.Error.UnauthorizedAccess"},
        "Unauthorized access: Application does not have permission to access "
        "system time."));
    reply.send();
    return timestamp;
  } else {
    // return D-Bus reply with timestamp
    timestamp = (uint64_t)(std::time(nullptr));
#ifdef DEBUG
    std::cout << "Current time: " << timestamp << std::endl;
#endif
    auto reply = call.createReply();
    reply << (uint64_t)(timestamp);
    reply.send();
  }

  return timestamp;
}

int main(int argc, char *argv[]) {
  // create D-Bus connection to the session bus
  // and requests a well-known name on it
  sdbus::ServiceName ts_name{"com.system.time"};
  auto ts_connection = sdbus::createSessionBusConnection(ts_name);
  sdbus::ObjectPath ts_path{"/"};

  // create time server D-Bus object
  auto time_object = sdbus::createObject(*ts_connection, std::move(ts_path));

  sdbus::InterfaceName ts_interface{"com.system.time"};
  // register D-Bus methods on the permission server object
  time_object
      ->addVTable(sdbus::MethodVTableItem{
          sdbus::MethodName{"GetSystemTime"}, /* method name */
          sdbus::Signature{""},               /* input signature */
          {},                                 /* input parametres names */
          sdbus::Signature{"t"},              /* output signature */
          {},                                 /* output parametres names */
          &GetSystemTime,                     /* method callback function */
          {}                                  /* flags */
      })
      .forInterface(ts_interface);

  // run the loop on the connection
  ts_connection->enterEventLoop();
}
