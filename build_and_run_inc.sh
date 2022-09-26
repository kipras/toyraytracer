#!/usr/bin/env bash

#    `SDL2-2.24.0/x86_64-w64-mingw32/bin/sdl2-config --cflags --libs` \

CCOPTS=""
CCOPTS="${CCOPTS} -g"
CCOPTS="${CCOPTS} -std=c11"
# OPTS="${CCOPTS} -O3"

"${CC}" \
    ${CCOPTS} \
    'main.c' \
    -o 'main.exe' \
    -L'SDL2-2.24.0/x86_64-w64-mingw32/lib' \
    -lmingw32 -lSDL2main -lSDL2 -mwindows \
    \
    && ./main.exe
