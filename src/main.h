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

#define log_err(...) fprintf(stderr, __VA_ARGS__);

#define WINDOW_WIDTH        400
#define WINDOW_HEIGHT       400

// How many vertical/horizontal pixels will be rendered and averaged to produce one resulting pixel, when anti-aliasing.
// For example, if ANTIALIAS_FACTOR is 2 - then 4 pixels (2 by 2) will be rendered to produce one resulting pixel.
//
// Set ANTIALIAS_FACTOR to 1 to disable anti-aliasing.
#define ANTIALIAS_FACTOR    2


typedef struct App_s            App;


typedef enum {
    CC_z_0,
    CC_z_15_downwards,
    CC_down__fov_40,
} CameraConfig;

#define CAMERA_CONFIG   CC_z_15_downwards


typedef enum {
    SC_none,                                // No predefined scene.
    SC_6_spheres_fov_90,
    SC_6_spheres_fov_40,
    SC_6_spheres__fov_40__cam_z_0,
    SC_6_spheres__fov_40__cam_z_15_downwards,
    SC_camera_testing_1_sphere_fov_90,
    SC_camera_testing_4_spheres_fov_90,
    SC_camera_testing_4_spheres_fov_40,
} SceneConfig;

#define SCENE_CONFIG    SC_6_spheres__fov_40__cam_z_15_downwards


typedef enum {
    SK_none,                                // No predefined sky.
    SK_ambient_gray_07,
    SK_gradient_blue,
    SK_ambient_blue,
} SkyConfig;

#define SKY_CONFIG      SK_ambient_gray_07


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
};


void scene_6_spheres__fov_90(App *app);
void scene_6_spheres__fov_40__cam_z_0(App *app);
void scene_6_spheres__fov_40__cam_z_15_downwards(App *app);
void scene_camera_testing_1_sphere__fov_90(App *app);
void scene_camera_testing_4_spheres__fov_90(App *app);
void scene_camera_testing_4_spheres__fov_40(App *app);
void sky_ambient_gray_07(App *app);
void sky_gradient_blue(App *app);
void sky_ambient_blue(App *app);

#endif // __MAIN_H__
