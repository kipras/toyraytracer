#ifndef __SCENE_H__
#define __SCENE_H__

/**
 * Various pre-defined scene/camera/sky configurations that can be used.
 */

#define SCENE_SPHERES_MAX   20


typedef struct Scene_s          Scene;


#include <stdint.h>

#include "sphere.h"


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


struct Scene_s {
    Sphere         *spheres;
    uint32_t        spheresLength;
};


void init_scene(Scene *scene);

#endif // __SCENE_H__
