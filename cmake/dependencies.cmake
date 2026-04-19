# assimp glog glm 这三个 repo 现在还是要编译的
# 其中 glm 其实可以不编译直接用 header，这个编译是为了复用
# 我们在这里先 find package，如果失败了就从 third party 下面编译安装

set(THIRD_PARTY_DIR ${PROJECT_SOURCE_DIR}/third_party)

# helper: 检查 submodule 目录是否已初始化
macro(check_submodule_initialized name path)
    if(NOT EXISTS "${path}/CMakeLists.txt")
        message(FATAL_ERROR
            "${name} submodule not initialized. Run:\n"
            "  git submodule update --init --recursive")
    endif()
endmacro()

# helper: configure + build + install submodule 到 CMAKE_INSTALL_PREFIX
macro(build_and_install_submodule name source_dir)
    message(STATUS "${name} not found, building from submodule")
    check_submodule_initialized(${name} ${source_dir})
    execute_process(
        COMMAND ${CMAKE_COMMAND} -S ${source_dir} -B ${source_dir}/build
            -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
            -DCMAKE_BUILD_TYPE=Release
            ${ARGN}
        COMMAND_ERROR_IS_FATAL ANY
    )
    execute_process(
        COMMAND ${CMAKE_COMMAND} --build ${source_dir}/build --config Release
        COMMAND_ERROR_IS_FATAL ANY
    )
    execute_process(
        COMMAND ${CMAKE_COMMAND} --install ${source_dir}/build
        COMMAND_ERROR_IS_FATAL ANY
    )
endmacro()

# ====================== glm ======================
find_package(glm QUIET)
if(NOT glm_FOUND)
    build_and_install_submodule(glm ${THIRD_PARTY_DIR}/glm
        -DGLM_BUILD_TESTS=OFF
        -DBUILD_SHARED_LIBS=OFF
    )
    find_package(glm REQUIRED)
endif()

# ====================== glog ======================
find_package(glog QUIET)
if(NOT glog_FOUND)
    build_and_install_submodule(glog ${THIRD_PARTY_DIR}/glog
        -DBUILD_TESTING=OFF
    )
    find_package(glog REQUIRED)
endif()

# ====================== assimp ======================
find_package(assimp QUIET)
if(NOT assimp_FOUND)
    build_and_install_submodule(assimp ${THIRD_PARTY_DIR}/assimp
        -DBUILD_SHARED_LIBS=ON
        -DASSIMP_BUILD_TESTS=OFF
        -DASSIMP_BUILD_ASSIMP_TOOLS=OFF
    )
    find_package(assimp REQUIRED)
endif()
