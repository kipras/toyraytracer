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


typedef enum {
    CC_z_0,
    CC_z_15_downwards,
    CC_down__fov_40,
} CameraConfig;

#define CAMERA_CONFIG   CC_z_15_downwards


typedef enum {
    // No predefined scene.
    SC_none,

    // Standard 6 sphere scene. FOV 90.
    SC_6_spheres__fov_90,

    // Standard 6 sphere scene. FOV 40. Camera origin and direction z = 0 (camera looking parallel to y axis).
    // At FOV 40 the bottom spheres only partially fit in the scene.
    SC_6_spheres__fov_40__cam_z_0,

    // Standard 6 sphere scene. FOV 40. Camera origin z = 15 (slightly above ground) and looking slightly downwards.
    // This produces a scene that is similar to the original FOV 90 scene (at FOV 90).
    SC_6_spheres__fov_40__cam_z_15_downwards,

    // Similar to SC_6_spheres__fov_40__cam_z_15_downwards, but some of the spheres are metal.
    // Also, spheres have other colors (than just pure red/green/blue),
    SC_6_spheres__fov_40__cam_z_15_downwards_v2,

    // Similar to SC_6_spheres__fov_40__cam_z_15_downwards, but some of the spheres are metal/glass.
    // Also, spheres have other colors (than just pure red/green/blue),
    SC_6_spheres__fov_40__cam_z_15_downwards_v3,

    // Similar to SC_6_spheres__fov_40__cam_z_15_downwards_v3, except:
    // * has an additional metal (mirror) sphere at middle right.
    // * the top right green sphere is brushed metal (instead of matte).
    SC_7_spheres__fov_40__cam_z_15_downwards,

    SC_camera_testing_1_sphere_fov_90,
    SC_camera_testing_4_spheres_fov_90,
    SC_camera_testing_4_spheres_fov_40,

    SC_rt_testing__1_sphere_center__fov_40,
    SC_rt_testing__1_sphere_inside__fov_40,
} SceneConfig;

#define SCENE_CONFIG    SC_7_spheres__fov_40__cam_z_15_downwards


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
    SDL_Window     *sdlWindow;
    SDL_Renderer   *sdlRenderer;
    SDL_Texture    *sdlTexture;

    uint32_t        windowWidth;        // The final displayed image width.
    uint32_t        windowHeight;       // The final displayed image height.

    Scene           scene;
    Camera          camera;
};


void scene_6_spheres__fov_90(App *app);
void scene_6_spheres__fov_40__cam_z_0(App *app);
void scene_6_spheres__fov_40__cam_z_15_downwards(App *app);
void scene_6_spheres__fov_40__cam_z_15_downwards_v2(App *app);
void scene_6_spheres__fov_40__cam_z_15_downwards_v3(App *app);
void scene_7_spheres__fov_40__cam_z_15_downwards(App *app);
void scene_camera_testing_1_sphere__fov_90(App *app);
void scene_camera_testing_4_spheres__fov_90(App *app);
void scene_camera_testing_4_spheres__fov_40(App *app);
void scene_rt_testing__1_sphere_center__fov_40(App *app);
void scene_rt_testing__1_sphere_inside__fov_40(App *app);
void sky_ambient_gray_07(App *app);
void sky_gradient_blue(App *app);
void sky_ambient_blue(App *app);

#endif // __MAIN_H__
