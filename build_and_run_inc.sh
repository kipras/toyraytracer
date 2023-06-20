#!/usr/bin/env bash

# Exit when any command fails.
set -e


DIR=$(realpath $(dirname "${0}"))
SRC_DIR="src"

#    `SDL2-2.24.0/x86_64-w64-mingw32/bin/sdl2-config --cflags --libs` \

if [[ ${ENV_LINUX} == 1 ]]; then
    echo "#define ENV_LINUX 1" > "envconfig.h"
else
    echo "#define ENV_LINUX 0" > "envconfig.h"
fi

CCOPTS=""
CCOPTS="${CCOPTS} -g"
CCOPTS="${CCOPTS} -std=c11"
CCOPTS="${CCOPTS} -Wall"
CCOPTS="${CCOPTS} -Werror"
CCOPTS="${CCOPTS} -Wextra"
# CCOPTS="${CCOPTS} -pedantic"
# CCOPTS="${CCOPTS} -O2"
# CCOPTS="${CCOPTS} -O3"

LINKEROPTS=""
if [[ ${ENV_LINUX} == 1 ]]; then
    LINKEROPTS="-L SDL2-2.24.0/build/build/.libs -lSDL2main -lSDL2 -Wl,-rpath,${DIR}/SDL2-2.24.0/build/build/.libs -lm"
else
    LINKEROPTS='-L SDL2-devel-2.24.0-mingw/x86_64-w64-mingw32/lib -lmingw32 -lSDL2main -lSDL2 -mwindows'
fi

MAIN_BIN=""
if [[ ${ENV_LINUX} == 1 ]]; then
    MAIN_BIN="./${SRC_DIR}/main"
else
    MAIN_BIN="./${SRC_DIR}/main.exe"
fi

"${CC}" \
    ${CCOPTS} \
    -c "${SRC_DIR}/main.c" \
    -o "${SRC_DIR}/main.o"

"${CC}" \
    ${CCOPTS} \
    -c "${SRC_DIR}/ray.c" \
    -o "${SRC_DIR}/ray.o"

"${CC}" \
    ${CCOPTS} \
    "${SRC_DIR}/main.o" "${SRC_DIR}/ray.o" \
    -o ${MAIN_BIN} \
    ${LINKEROPTS} \
    \
    && ${MAIN_BIN}
