get_filename_component(fg_CMAKE_DIR ${CMAKE_CURRENT_LIST_FILE} PATH)

# 只有 glm 和 glad 暴露给外面
# 但是 glad 不是 cmake package
find_package(glm REQUIRED)
include(${fg_CMAKE_DIR}/fg-targets.cmake)
set(fg_FOUND true)