#!/usr/bin/env bash

# Exit when any command fails.
set -e


DIR=$(realpath $(dirname "${0}"))

#    `SDL2-2.24.0/x86_64-w64-mingw32/bin/sdl2-config --cflags --libs` \

if [[ ${ENV_LINUX} == 1 ]]; then
    echo "#define ENV_LINUX 1" > envconfig.h
else
    echo "#define ENV_LINUX 0" > envconfig.h
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
    MAIN_BIN='./main'
else
    MAIN_BIN='./main.exe'
fi

"${CC}" \
    ${CCOPTS} \
    -c 'main.c' \
    -o 'main.o'

"${CC}" \
    ${CCOPTS} \
    -c 'ray.c' \
    -o 'ray.o'

"${CC}" \
    ${CCOPTS} \
    'main.o' 'ray.o' \
    -o ${MAIN_BIN} \
    ${LINKEROPTS} \
    \
    && ${MAIN_BIN}
