find_package(absl COMPONENTS debugging failure_signal_handler PATHS ${CMAKE_SOURCE_DIR}/third_party/install)

add_library(utils STATIC
    src/filesystem.cpp
    src/logging.cpp)

target_include_directories(utils
    PUBLIC inc)
target_link_libraries(utils
    PUBLIC absl::debugging
    PUBLIC absl::failure_signal_handler)