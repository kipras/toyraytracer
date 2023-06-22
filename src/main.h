#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdbool.h>

#include "../envconfig.h"
#if defined(ENV_LINUX) && ENV_LINUX
#include "../SDL2-2.24.0/include/SDL.h"
#else
#include "../SDL2-devel-2.24.0-mingw/x86_64-w64-mingw32/include/SDL2/SDL.h"
#endif // ENV_LINUX


// #define DEBUG 1
#define DEBUG 0

#define DEBUG_ANGLE_TO_CC   DEBUG && 1

#define dbg(...) if (DEBUG) { printf(__VA_ARGS__); }

#define assert2(exp, msg) if (! (exp)) { printf("%s", msg); exit(1); }

#define WINDOW_WIDTH        400
#define WINDOW_HEIGHT       400

// How many vertical/horizontal pixels will be rendered and averaged to produce one resulting pixel, when anti-aliasing.
// For example, if ANTIALIAS_FACTOR is 2 - then 4 pixels (2 by 2) will be rendered to produce one resulting pixel.
//
// Set ANTIALIAS_FACTOR to 1 to disable anti-aliasing.
#define ANTIALIAS_FACTOR    2

#define FOV                 90


// // Wrappers around malloc()/free(). We call these (instead of calling free() directly) just in case we will decide to replace the standard
// // malloc()/free() with some other memory allocator later on - the we would only need to replace them here.
// #define ralloc(sz)          malloc(sz)
// #define rfree(p)            free(p)


typedef struct App_s            App;


#include "ray.h"
#include "types.h"


struct App_s {
    SDL_Window     *window;
    SDL_Renderer   *renderer;

    uint32_t        windowWidth;
    uint32_t        windowHeight;

    Scene           scene;
    Ray             camera;

    // // Precalculated camera angles for screen pixels
    // Angle           renderWindowPixelsHorAngles[WINDOW_WIDTH];
    // Angle           renderWindowPixelsVertAngles[WINDOW_HEIGHT];

    // // Direction       renderWindowPixelsCameraDirectionCache[WINDOW_HEIGHT * WINDOW_WIDTH];

    // // FOV can theoretically go up to the full 360 degrees. However, practically it usually goes up to no more than
    // // 120 degrees in graphics applications, so we use an 8 bit integer (up to 255) to store it.
    // uint8_t         fov;
};


void log_err(char *err);
void init_app(App *app);
void init_screen(App *app);
void init_world(App *app);
void add_sphere(App *app, Sphere sphere);
void run_render_loop(App *app);
void output_stats(App *app, struct timespec *tstart, uint64_t frames);
bool keyboard_key_pressed();
// float fast_inv_sqrt(float number);

#endif // __MAIN_H__