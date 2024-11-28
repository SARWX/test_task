cmake_minimum_required(VERSION 3.0)
project(permission_manager)

# find sdbus-c++ library
find_package(sdbus-c++ REQUIRED)

# create shared library permission_manager
add_library(permission_manager SHARED permission_manager.cpp)

# create static library permissions_db
add_library(permissions_db STATIC permissions_db.cpp)
target_link_libraries(permissions_db PRIVATE sqlite3)

# create executables
add_executable(server conv_server.cpp)
add_executable(client conv_client.cpp)
add_executable(time_server time_server.cpp)
add_executable(task_3 task_3.cpp)

# link libraries to executables
target_link_libraries(server PRIVATE SDBusCpp::sdbus-c++ permission_manager permissions_db sqlite3)
target_link_libraries(time_server PRIVATE SDBusCpp::sdbus-c++ permission_manager permissions_db sqlite3)
target_link_libraries(client PRIVATE SDBusCpp::sdbus-c++)
target_link_libraries(task_3 PRIVATE SDBusCpp::sdbus-c++)

# custom target to clean all build files
add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/*
    COMMENT "Cleaning all build files"
)
