cmake_minimum_required(VERSION 3.28.0)

option(VERBOSE "enable verbose log with detailed command" OFF)
if(VERBOSE)
    set(CMAKE_VERBOSE_MAKEFILE ON)
endif()

project(fg_lib)

set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(THIRD_PARTY_DIR ${PROJECT_SOURCE_DIR}/third_party)

file(GLOB_RECURSE SRC_CC_FILES ${SRC_DIR}/*.cc) 
foreach(var ${SRC_CC_FILES})
    message(${var})
endforeach()

# ====================== fg target ======================
# 如果不设置成默认可见，glad 里面的符号也需要标，这太麻烦了
add_library(fg SHARED ${SRC_CC_FILES})
set_target_properties(
    fg PROPERTIES
    CXX_VISIBILITY_PRESET default
)

target_include_directories(
    fg PUBLIC 
    ${SRC_DIR}
    ${INCLUDE_DIR} 
    ${THIRD_PARTY_DIR}
    ${THIRD_PARTY_DIR}/assimp/include
    ${THIRD_PARTY_DIR}/glog/src
    ${THIRD_PARTY_DIR}/glog/build
)
target_link_directories(
    fg PUBLIC
    ${THIRD_PARTY_DIR}/assimp/bin
    ${THIRD_PARTY_DIR}/glog/build
)
target_link_libraries(
    fg PRIVATE
    assimp
    glog
)

# platform dependency
if(WIN32)
    
elseif(APPLE)
    find_library(CocoaPATH Cocoa)
    find_library(IOKitPATH IOKit)
    find_library(OpenGLPATH OpenGL)

    target_link_libraries(
        fg PRIVATE
        ${CocoaPATH}
        ${IOKitPATH}
        ${OpenGLPATH}
    )
endif()


# ====================== compile flags ======================
add_library(build_flags INTERFACE)

target_compile_features(build_flags INTERFACE cxx_std_14)
target_compile_definitions(build_flags INTERFACE FG_EXPORT)
# when using glog, must define GLOG_USE_GLOG_EXPORT or compile fail
target_compile_definitions(build_flags INTERFACE GLOG_USE_GLOG_EXPORT)

option(USE_VLOG "use vlog in fg" OFF)
if (USE_VLOG)
    target_compile_definitions(build_flags INTERFACE USE_VLOG)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(build_flags INTERFACE -w)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(build_flags INTERFACE /W0)
endif()

target_link_libraries(fg PUBLIC build_flags)
