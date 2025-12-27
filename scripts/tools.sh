#!/bin/bash

# ============================================================
# 使用 parse_install_args 分析命令行参数：
# parse_install_args : 无事发生
# parse_install_args install : 设置 CMAKE_TARGET_STR
# parse_install_args install {YOUR_PATH} : 额外设置 CMAKE_INSTALL_PREFIX_STR，CMAKE_PREFIX_STR

# ============================================================
parse_install_args() {
    local DO_INSTALL="false"
    local GIVEN_PATH="false"
    local INSTALL_PATH=""
    
    if [ $# -ge 1 ]; then
        if [ "$1" = "install" ]; then
            DO_INSTALL="true"
            echo "do install!"
            if [ $# -ge 2 ]; then
                GIVEN_PATH="true"
                INSTALL_PATH="$2"
                echo "install path: $INSTALL_PATH"
            fi
        fi
    fi
    
    CMAKE_INSTALL_PREFIX_STR=""
    CMAKE_PREFIX_STR=""
    if [ "$DO_INSTALL" = "true" ] && [ "$INSTALL_PATH" != "" ]; then
        CMAKE_INSTALL_PREFIX_STR="-DCMAKE_INSTALL_PREFIX=$INSTALL_PATH"
        CMAKE_PREFIX_STR="-DCMAKE_PREFIX_PATH=$INSTALL_PATH"
    fi
    
    CMAKE_TARGET_STR=""
    if [ "$DO_INSTALL" = "true" ]; then
        CMAKE_TARGET_STR="--target install"
    fi
}
