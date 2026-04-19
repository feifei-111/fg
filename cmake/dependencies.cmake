# assimp glog glm 这三个 repo 现在还是要编译的
# 其中 glm 其实可以不编译直接用 header，这个编译是为了复用
# 我们在这里先 find package，如果失败了就从 third part 下面编译

set(THIRD_PARTY_DIR ${PROJECT_SOURCE_DIR}/third_party)

# helper: 检查 submodule 目录是否已初始化
macro(check_submodule_initialized name path)
    if(NOT EXISTS "${path}/CMakeLists.txt")
        message(FATAL_ERROR
            "${name} submodule not initialized. Run:\n"
            "  git submodule update --init --recursive")
    endif()
endmacro()

# ====================== glm ======================
find_package(glm QUIET)
if(NOT glm_FOUND)
    message(STATUS "glm not found, building from submodule")
    check_submodule_initialized(glm ${THIRD_PARTY_DIR}/glm)
    set(GLM_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    add_subdirectory(${THIRD_PARTY_DIR}/glm)
endif()

# ====================== glog ======================
find_package(glog QUIET)
if(NOT glog_FOUND)
    message(STATUS "glog not found, building from submodule")
    check_submodule_initialized(glog ${THIRD_PARTY_DIR}/glog)
    set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
    add_subdirectory(${THIRD_PARTY_DIR}/glog)
endif()

# ====================== assimp ======================
find_package(assimp QUIET)
if(NOT assimp_FOUND)
    message(STATUS "assimp not found, building from submodule")
    check_submodule_initialized(assimp ${THIRD_PARTY_DIR}/assimp)
    set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
    add_subdirectory(${THIRD_PARTY_DIR}/assimp)
endif()
