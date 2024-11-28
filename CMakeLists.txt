cmake_minimum_required(VERSION 3.0)
project(permission_manager)

# First, find sdbus-c++
find_package(sdbus-c++ REQUIRED)

# add_executable(hello hello.cpp)
# add_executable(server server.cpp)
add_executable(server conv_server.cpp)
# add_executable(client client.cpp)
add_executable(client conv_client.cpp)

add_executable(time_server time_server.cpp)

add_executable(task_3 task_3.cpp)


add_library(permissions_db STATIC permissions_db.cpp)
target_link_libraries(server PRIVATE SDBusCpp::sdbus-c++ permissions_db sqlite3)



# Use the sdbus-c++ target in SDBusCpp namespace
# target_link_libraries(hello PRIVATE SDBusCpp::sdbus-c++)
target_link_libraries(server PRIVATE SDBusCpp::sdbus-c++)
target_link_libraries(client PRIVATE SDBusCpp::sdbus-c++)
target_link_libraries(time_server PRIVATE SDBusCpp::sdbus-c++)
target_link_libraries(task_3 PRIVATE SDBusCpp::sdbus-c++)

target_link_libraries(permissions_db PRIVATE sqlite3)


add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/*
    COMMENT "Cleaning all files in build directory"
)
