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


typedef struct App_s            App;


typedef enum {
    SC_none,                                // No predefined scene.
    SC_6_spheres_fov_90,
    SC_6_spheres_fov_40,
    SC_camera_testing_1_sphere_fov_90,
    SC_camera_testing_4_spheres_fov_90,
    SC_camera_testing_4_spheres_fov_40,
} SceneConfig;

typedef enum {
    SK_none,                                // No predefined sky.
    SK_ambient_07,
    SK_gradient_blue,
    SK_ambient_blue,
} SkyConfig;


#include "camera.h"
#include "ray.h"
#include "types.h"


struct App_s {
    SDL_Window     *window;
    SDL_Renderer   *renderer;

    uint32_t        windowWidth;
    uint32_t        windowHeight;

    Scene           scene;
    Camera          camera;

    // // Precalculated camera angles for screen pixels
    // Angle           renderWindowPixelsHorAngles[WINDOW_WIDTH];
    // Angle           renderWindowPixelsVertAngles[WINDOW_HEIGHT];

    // // Direction       renderWindowPixelsCameraDirectionCache[WINDOW_HEIGHT * WINDOW_WIDTH];

    // // FOV can theoretically go up to the full 360 degrees. However, practically it usually goes up to no more than
    // // 120 degrees in graphics applications, so we use an 8 bit integer (up to 255) to store it.
    // uint8_t         fov;
};


void log_err(char *err);
void scene_6_spheres_fov_90(App *app);
void scene_6_spheres_fov_40(App *app);
void scene_camera_testing_1_sphere_fov_90(App *app);
void scene_camera_testing_4_spheres_fov_90(App *app);
void scene_camera_testing_4_spheres_fov_40(App *app);
void sky_ambient_07(App *app);
void sky_gradient_blue(App *app);
void sky_ambient_blue(App *app);

#endif // __MAIN_H__
