cmake_minimum_required(VERSION 3.0)
project(Hello_proj)

# First, find sdbus-c++
find_package(sdbus-c++ REQUIRED)

# add_executable(hello hello.cpp)
add_executable(server server.cpp)
add_executable(client client.cpp)

add_executable(sql_test sql_test.cpp)


# Use the sdbus-c++ target in SDBusCpp namespace
# target_link_libraries(hello PRIVATE SDBusCpp::sdbus-c++)
target_link_libraries(server PRIVATE SDBusCpp::sdbus-c++)
target_link_libraries(client PRIVATE SDBusCpp::sdbus-c++)

target_link_libraries(sql_test PRIVATE sqlite3)


add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/*
    COMMENT "Cleaning all files in build directory"
)
