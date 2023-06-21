#!/usr/bin/env bash

# Exit when any command fails.
set -e


DIR=$(realpath $(dirname "${0}"))
SRC_DIR="src"

#    `SDL2-2.24.0/x86_64-w64-mingw32/bin/sdl2-config --cflags --libs` \

MAIN_BIN=""
if [[ ${ENV_LINUX} == 1 ]]; then
    MAIN_BIN="${SRC_DIR}/main"
else
    MAIN_BIN="${SRC_DIR}/main.exe"
fi

CC=${CC} ENV_LINUX=${ENV_LINUX} make -j8

${MAIN_BIN}
