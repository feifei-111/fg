cmake_minimum_required(VERSION 3.28.0)
include(GNUInstallDirs)
project(fg_test)

set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/install)
set(CMAKE_INSTALL_RPATH "${CMAKE_PREFIX_PATH}/lib")

add_executable(fg_example main.cc)
find_package(fg REQUIRED)
target_link_libraries(
    fg_example PUBLIC
    fg::fg
)

install(
    TARGETS fg_example
    RUNTIME DESTINATION .
)
install(
    DIRECTORY ${CMAKE_SOURCE_DIR}/assets/
    DESTINATION ./assets
)