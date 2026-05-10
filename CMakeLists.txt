cmake_minimum_required(VERSION 3.28.0)
include(GNUInstallDirs)

# 统一所有 target 使用动态 CRT (/MD)，避免跨 DLL 边界的 ABI 不一致
cmake_policy(SET CMP0091 NEW)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

project(fg_lib)

# 设置产物默认路径，这个设置需要放在 project 之后
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)


set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(THIRD_PARTY_DIR ${PROJECT_SOURCE_DIR}/third_party)
set(CMAKE_CONFIG_DIR ${PROJECT_SOURCE_DIR}/cmake)

# ====================== find source ======================
file(GLOB_RECURSE SRC_CC_FILES ${SRC_DIR}/*.cc) 

if(WIN32)
    list(FILTER SRC_CC_FILES EXCLUDE REGEX ".*[/\\\\]glfw[/\\\\].*")
else()
    list(FILTER SRC_CC_FILES EXCLUDE REGEX ".*[/\\\\]win32[/\\\\].*")
endif()

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

# 不用指定 install interface 的 ${CMAKE_INSTALL_INCLUDEDIR}，默认添加
target_include_directories(
    fg PUBLIC
    $<BUILD_INTERFACE:${INCLUDE_DIR}>
    $<BUILD_INTERFACE:${SRC_DIR}>
    $<BUILD_INTERFACE:${THIRD_PARTY_DIR}>
)

# ====================== 依赖的 third party ======================
include(cmake/dependencies.cmake)

# glm 可以暴露依赖，因为可能确实要用 glm 接口
# glm 提前编了一些模版实现，有 glm::glm 和 glm::glm-header-only 两个空间
target_link_libraries(
    fg PUBLIC
    glm::glm-header-only
)

target_link_libraries(
    fg PRIVATE
    assimp::assimp
    glog::glog
)

# stb 和 whereami 没办法，只能手动加 include 了，他们没法 find package
target_include_directories(
    fg PUBLIC
    $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/stb>
    $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/whereami/src>
)

# whereami 还有一个 .c 要一起编
target_sources(
    fg
    PRIVATE
    ${THIRD_PARTY_DIR}/whereami/src/whereami.c
)

# ====================== 依赖的系统 lib ======================
if(WIN32)
    target_link_libraries(
        fg PRIVATE
        opengl32
        gdi32
        user32
        kernel32
        shell32
        advapi32
        ole32
        ws2_32
        # glu32
    )

    target_compile_definitions(fg PRIVATE
        UNICODE
    )
    
    # executable file need this link option, skip for dylib
    # if(MSVC)
    #     target_link_options(fg PRIVATE
    #         "/SUBSYSTEM:WINDOWS"
    #         "/ENTRY:mainCRTStartup"
    #     )
    # endif()

elseif(APPLE)
    find_package(glfw3 REQUIRED)

    # os 自带的 framwork 可以直接用这个字符串指定
    # 本质上 target_link_libraries 就是配置给 linker 的字符串
    target_link_libraries(
        fg PRIVATE
        "-framework Cocoa"
        "-framework IOKit"
        "-framework OpenGL"
        glfw
    )
    # glfw 搜的时候用 glfw3，用的时候用 glfw，也没有命名空间，有点怪
endif()


# ====================== build flags ======================
add_library(build_flags INTERFACE)

target_compile_features(build_flags INTERFACE cxx_std_20)

option(USE_VLOG "use vlog in fg" OFF)
if (USE_VLOG)
    target_compile_definitions(build_flags INTERFACE USE_VLOG)
endif()

# no warning
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(build_flags INTERFACE -w)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(build_flags INTERFACE /W0)
endif()

target_link_libraries(fg PUBLIC build_flags)

# ====================== export flags ======================
add_library(export_flags INTERFACE)
# when using glog and fg is dylib, so need define EXPORT flag (for win)
target_compile_definitions(export_flags INTERFACE
    FG_USE_FG_EXPORT
    FG_EXPORT_BUILD
    GLOG_USE_GLOG_EXPORT
    GLOG_NO_ABBREVIATED_SEVERITIES
    GLAD_API_CALL_EXPORT
    GLAD_API_CALL_EXPORT_BUILD
)
# 这个私有，不应该导出
target_link_libraries(fg PRIVATE export_flags)

# glad 消费方需要 GLAD_API_CALL_EXPORT 才能 dllimport glad 符号
if(WIN32)
    target_compile_definitions(fg PUBLIC
        FG_USE_FG_EXPORT
        GLAD_API_CALL_EXPORT
    )
endif()

# ====================== install ======================
# install fg
# glog glm assimp 默认已经 install 了
install(TARGETS fg build_flags
        EXPORT fg-targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT fg-targets
        FILE fg-targets.cmake
        NAMESPACE fg::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/fg
)

# 这里需要加一个斜杠，表示将 INCLUDE_DIR 下面的所有文件拷贝到 include 下面
# 否则这里会变成 include/include
install(DIRECTORY ${INCLUDE_DIR}/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(DIRECTORY ${CMAKE_CONFIG_DIR}/
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/fg
)

# install 其他 third party，主要是自己没有 cmake 的
install(DIRECTORY ${THIRD_PARTY_DIR}/glad
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# 可以不安装下面两个，因为没有把他们的头文件暴露出来
# install(FILES ${THIRD_PARTY_DIR}/stb/stb_image.h
#     DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
# )
# install(FILES ${THIRD_PARTY_DIR}/whereami/src/whereami.h
#     DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
# )


