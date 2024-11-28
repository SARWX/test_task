#include "permission_manager.h"
#include <iostream>
#include <sdbus-c++/sdbus-c++.h>
#include <string>

int main(int argc, char *argv[]) {
  // create proxy object for time server
  sdbus::ServiceName ts_name{"com.system.time"};
  sdbus::ObjectPath ts_path{"/"};
  auto ts_proxy = sdbus::createProxy(std::move(ts_name), std::move(ts_path));
  sdbus::InterfaceName ts_interface{"com.system.time"};

  // create proxy object for permission server
  sdbus::ServiceName ps_name{"com.system.permissions"};
  sdbus::ObjectPath ps_path{"/"};
  auto ps_proxy = sdbus::createProxy(std::move(ps_name), std::move(ps_path));
  sdbus::InterfaceName ps_interface{"com.system.permissions"};

  enum Permissions time_permision = SystemTime;
  uint64_t timestamp = 0;
  std::string error_name;

  try {
    ts_proxy->callMethod("GetSystemTime")
        .onInterface(ts_interface)
        .storeResultsTo(timestamp);
    std::cout << "Current time: " << std::ctime((time_t *)&timestamp);
  } catch (const sdbus::Error &e) {
    error_name = e.getName();
    std::cerr << "Error while calling GetSystemTime: " << error_name << " - "
              << e.getMessage() << std::endl;
  }

  if ((timestamp == 0) &&
      (error_name == "com.system.time.Error.UnauthorizedAccess")) {
    try {
      ps_proxy->callMethod("RequestPermission")
          .onInterface(ps_interface)
          .withArguments((int)time_permision);
      ts_proxy->callMethod("GetSystemTime")
          .onInterface(ts_interface)
          .storeResultsTo(timestamp);
      std::cout << "Current time: " << std::ctime((time_t *)&timestamp);
    } catch (const sdbus::Error &e) {
      std::cerr
          << "Error while requesting permission or calling GetSystemTime: "
          << e.getName() << " - " << e.getMessage() << std::endl;
    }
  }

  return 0;
}
