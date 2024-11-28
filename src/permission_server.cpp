#include "permission_manager.h"
#include "permissions_db.h"
#include <iostream>
#include <limits.h>
#include <sdbus-c++/sdbus-c++.h>
#include <string>
#include <unistd.h>

static permissions_db *g_db = nullptr;

void RequestPermission(sdbus::MethodCall call) {
  // get pid from message
  pid_t current_pid = call.getCredsPid();

  // get permission_code
  int permission_code;
  call >> permission_code;
#ifdef DEBUG
  std::cout << std::endl << permission_code << std::endl;
#endif

  // make shure permission_code is valid
  if (!is_valid_permission(permission_code))
    throw sdbus::Error(sdbus::Error::Name{"com.system.permissions.Error"},
                       "Not valid permission code");
#ifdef DEBUG
  std::cout << current_pid << std::endl;
#endif

  // convert PID to system path
  char path[PATH_MAX];
  pid_to_path(current_pid, path, PATH_MAX);
#ifdef DEBUG
  std::cout << "Path of sender: " << path << std::endl;
#endif

  // insert data into database
  g_db->insert_permission(path, permission_code);

  // send empty reply
  auto reply = call.createReply();
  reply.send();
}

bool CheckApplicationHasPermission(std::string applicationExecPath,
                                   int permissionEnumCode) {
  return (
      g_db->check_permission(applicationExecPath.c_str(), permissionEnumCode));
}

int main(int argc, char *argv[]) {
  // create D-Bus connection to the session bus
  // and requests a well-known name on it
  sdbus::ServiceName ps_name{"com.system.permissions"};
  auto ps_connection = sdbus::createSessionBusConnection(ps_name);

  // create permission server D-Bus object
  sdbus::ObjectPath objectPath{"/"};
  auto permission_server =
      sdbus::createObject(*ps_connection, std::move(objectPath));

  sdbus::InterfaceName ps_interface{"com.system.permissions"};
  // register D-Bus methods on the permission server object
  permission_server
      ->addVTable(sdbus::MethodVTableItem{
          sdbus::MethodName{"RequestPermission"}, /* method name */
          sdbus::Signature{"i"},                  /* input signature */
          {},                                     /* input parametres names */
          sdbus::Signature{},                     /* output signature */
          {},                                     /* output parametres names */
          RequestPermission,                      /* method callback function */
          {}                                      /* flags */
      })
      .forInterface(ps_interface);

  permission_server
      ->addVTable(sdbus::registerMethod("CheckApplicationHasPermission")
                      .implementedAs(CheckApplicationHasPermission))
      .forInterface(ps_interface);

  // create permissions_db object
  permissions_db db("permissions.db");
  // make it global (within this file)
  g_db = &db;

  // run the loop on the connection
  ps_connection->enterEventLoop();
}
