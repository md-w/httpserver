cmake_minimum_required(VERSION 3.16)
project(http_server)
set(CMAKE_EXPORT_COMPILE_COMMANDS)

find_package(Poco COMPONENTS Foundation Util Net PATHS "third_party/install")
find_package(Protobuf REQUIRED PATHS ${CMAKE_SOURCE_DIR}/3rdparty/install)

add_subdirectory(utils)

add_executable(http_server 
    src/EntryPoint.cpp
    src/LogReceiverServer.cpp
    src/status.pb.cc)

target_link_libraries(http_server 
    PUBLIC Poco::Foundation
    PUBLIC Poco::Util
    PUBLIC Poco::Net
    PUBLIC utils
    PUBLIC protobuf)

target_include_directories(http_server
    PUBLIC inc)