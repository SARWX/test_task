cmake_minimum_required(VERSION 3.14)
project(permission_manager)

# find sdbus-c++ library
find_package(sdbus-c++ REQUIRED)
# find the SQLite libraries
FindSQLite3

# add debug information if needed
# add_compile_definitions(DEBUG)

# specify include directory
include_directories(./inc)

# create shared library permission_manager
add_library(permission_manager SHARED lib/permission_manager.cpp)

# create shared library permissions_db
add_library(permissions_db SHARED lib/permissions_db.cpp)
target_link_libraries(permissions_db PRIVATE sqlite3)

# create executables
add_executable(permission_server src/permission_server.cpp)
add_executable(time_server src/time_server.cpp)
add_executable(task_3 src/task_3.cpp)

# link libraries to executables
target_link_libraries(permission_server PRIVATE SDBusCpp::sdbus-c++ permission_manager permissions_db sqlite3)
target_link_libraries(time_server PRIVATE SDBusCpp::sdbus-c++ permission_manager permissions_db sqlite3)
target_link_libraries(task_3 PRIVATE SDBusCpp::sdbus-c++)

# custom target to clean all build files
add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/*
    COMMENT "Cleaning all build files"
)
