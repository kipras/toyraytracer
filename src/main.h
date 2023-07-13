#ifndef __MAIN_H__
#define __MAIN_H__

// stdio.h is required for log_err() definition (fprintf and stderr).
#include <stdio.h>

#include "../envconfig.h"
#if defined(ENV_LINUX) && ENV_LINUX
#include "../SDL2-2.24.0/include/SDL.h"
#else
#include "../SDL2-devel-2.24.0-mingw/SDL2-2.24.0/x86_64-w64-mingw32/include/SDL2/SDL.h"
#endif // ENV_LINUX


// #define DEBUG 1
#define DEBUG 0

#define DEBUG_ANGLE_TO_CC   DEBUG && 1

#define dbg(...) if (DEBUG) { printf(__VA_ARGS__); }

#define assert2(exp, msg) if (! (exp)) { printf("%s", msg); exit(1); }

#define log_err(...) fprintf(stderr, __VA_ARGS__);

#define WINDOW_WIDTH        400
#define WINDOW_HEIGHT       400

// This configures the down-sampling (super-sampling) anti-aliasing.
// How many vertical/horizontal pixels will be rendered and averaged to produce one resulting pixel, when anti-aliasing.
// For example, if ANTIALIAS_FACTOR is 2 - then 4 pixels (2 by 2) will be rendered to produce one resulting pixel.
//
// Set ANTIALIAS_FACTOR to 1 to disable anti-aliasing.
//
// UPDATE: we don't use this anymore, but i kept the support for this anti-aliasing.
// Instead we now randomize the camera rays directions for each pixel (within pixel's boundaries) for each frame, see cam_frame_init().
// This performs much better and results in better anti-aliasing and better overall rendered image quality (after multiple frames are
// blended together).
#define ANTIALIAS_FACTOR    1


typedef struct App_s            App;


#include "camera.h"
#include "scene.h"


struct App_s {
    SDL_Window     *sdlWindow;
    SDL_Renderer   *sdlRenderer;
    SDL_Texture    *sdlTexture;

    uint32_t        windowWidth;        // The final displayed image width.
    uint32_t        windowHeight;       // The final displayed image height.

    Scene           scene;
    Camera          camera;
};

#endif // __MAIN_H__
