cmake_minimum_required(VERSION 3.20)
project(http_server)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # does not produce the json file
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "") # works

find_package(Poco COMPONENTS Foundation Util Net PATHS ${CMAKE_SOURCE_DIR}/third_party/install)
find_package(Protobuf COMPONENTS libprotobuf protoc PATHS ${CMAKE_SOURCE_DIR}/third_party/install)

add_subdirectory(utils)

# protobuf_generate(PROTO_SRCS PROTO_HDRS ${CMAKE_SOURCE_DIR}/docs/status.proto)

add_executable(http_server 
    src/EntryPoint.cpp
    src/LogReceiverServer.cpp
    src/LogReceiverTCPServerConnectionFactory.cpp
    src/CentralDataRepo.cpp

    src/OptionsRequestHandler.cpp
    src/NotFoundRequestHandler.cpp
    src/MethodNotSupportedRequestHandler.cpp
    src/GenericHTTPRequestHandlerFactory.cpp
    src/StatusRequestHandler.cpp

    src/FormRequestHandler.cpp
    src/FormRequestHandler.cpp
    src/MultiPartHandler.cpp
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

# add_subdirectory(resource)

add_library(resource
    resource/src/monitoring_thread.cpp
    )

target_include_directories(resource
    PUBLIC ${CMAKE_SOURCE_DIR}/resource/inc
    PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
    )

protobuf_generate(
    LANGUAGE cpp
    TARGET resource
    PROTOS ${CMAKE_SOURCE_DIR}/interfaces/status.proto
    )

target_link_libraries(resource
    PUBLIC Poco::Net
    PUBLIC protobuf::libprotobuf
    )


add_executable(resource_tester
    resource/src/EntryPoint.cpp
    resource/src/DummyDataGenerator.cpp
    )

target_link_libraries(resource_tester
    PUBLIC Poco::Util
    PUBLIC resource
    )