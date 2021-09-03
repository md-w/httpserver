cmake_minimum_required(VERSION 3.16)
project(http_server)
set(CMAKE_EXPORT_COMPILE_COMMANDS)

find_package(Poco COMPONENTS Foundation Util Net PATHS "third_party/install")
find_package(Protobuf COMPONENTS libprotobuf protoc PATHS ${CMAKE_SOURCE_DIR}/third_party/install)

add_subdirectory(utils)

# protobuf_generate(PROTO_SRCS PROTO_HDRS ${CMAKE_SOURCE_DIR}/docs/status.proto)

add_executable(http_server 
    src/EntryPoint.cpp
    src/LogReceiverServer.cpp
    src/CentralDataRepo.cpp
    )

target_include_directories(http_server
    PUBLIC inc
    PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
    )

protobuf_generate(
    LANGUAGE cpp
    TARGET http_server
    PROTOS ${CMAKE_SOURCE_DIR}/interfaces/status.proto
    )

target_link_libraries(http_server 
    PUBLIC Poco::Foundation
    PUBLIC Poco::Util
    PUBLIC Poco::Net
    PUBLIC utils
    PUBLIC protobuf::libprotobuf
    )