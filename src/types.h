#ifndef __TYPES_H__
#define __TYPES_H__


#define SCENE_SPHERES_MAX   20


typedef struct Sphere_s         Sphere;
typedef struct Scene_s          Scene;
// typedef struct App_s            App;
// typedef struct Point3_s         Point3;


#include "color.h"
#include "material.h"
#include "ray.h"
#include "vector.h"


struct Sphere_s {
    Vector3         center;
    uint32_t        radius;
//    double          radiusDiv2;
    Material       *material;
    Color           color;
};

struct Scene_s {
    Sphere          spheres[SCENE_SPHERES_MAX];
    uint32_t        spheresLength;

    // double          groundHeight;   // Vertical position of the ground.
    // Color           groundColor;

    // double          skyHeight;      // Vertical position of the sky.
    // Color           skyColor;
};

// struct App_s {
//     SDL_Window     *window;
//     SDL_Renderer   *renderer;

//     uint32_t        windowWidth;
//     uint32_t        windowHeight;

//     Scene           scene;
//     Ray             camera;

//     // // Precalculated camera angles for screen pixels
//     // Angle           renderWindowPixelsHorAngles[WINDOW_WIDTH];
//     // Angle           renderWindowPixelsVertAngles[WINDOW_HEIGHT];

//     // // Direction       renderWindowPixelsCameraDirectionCache[WINDOW_HEIGHT * WINDOW_WIDTH];

//     // // FOV can theoretically go up to the full 360 degrees. However, practically it usually goes up to no more than
//     // // 120 degrees in graphics applications, so we use an 8 bit integer (up to 255) to store it.
//     // uint8_t         fov;
// };

// struct Point3_s {
//     // 2D plane (if looking from the top) coordinates.
//     double x;
//     double y;

//     // Height coordinate.
//     double z;
// };

#endif // __TYPES_H__
