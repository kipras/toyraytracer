#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdbool.h>

#include "envconfig.h"
#if defined(ENV_LINUX) && ENV_LINUX
#include "SDL2-2.24.0/include/SDL.h"
#else
#include "SDL2-devel-2.24.0-mingw/x86_64-w64-mingw32/include/SDL2/SDL.h"
#endif // ENV_LINUX


// #define DEBUG 1
#define DEBUG 0

#define PI                  3.14159265

#define WINDOW_WIDTH        200
#define WINDOW_HEIGHT       200
#define FOV                 90
#define SCENE_SPHERES_MAX   20
#define RAY_BOUNCES_MAX     20

// // Wrappers around malloc()/free(). We call these (instead of calling free() directly) just in case we will decide to replace the standard
// // malloc()/free() with some other memory allocator later on - the we would only need to replace them here.
// #define ralloc(sz)          malloc(sz)
// #define rfree(p)            free(p)


typedef enum   MaterialType_e   MaterialType;
typedef struct Color_s          Color;
typedef struct Sphere_s         Sphere;
typedef struct Scene_s          Scene;
typedef struct App_s            App;
// typedef struct Point3_s         Point3;


#include "ray.h"
#include "vector.h"


enum MaterialType_e {
    Matte = 1,      // Diffuse ? Lambert ?
    Metal,          // ?? (Specular ??)
    Glass,          // Refractive ? Dielectric ?
};

struct Color_s {
    uint8_t     red;
    uint8_t     green;
    uint8_t     blue;
};
#define COLOR_BLACK {  0,   0,   0}
#define COLOR_RED   {255,   0,   0}
#define COLOR_GREEN {  0, 255,   0}
#define COLOR_BLUE  {  0,   0, 255}

struct Sphere_s {
    Vector3         center;
    uint32_t        radius;
//    double          radiusDiv2;
    MaterialType    materialType;
    Color           color;
};

struct Scene_s {
    Sphere          spheres[SCENE_SPHERES_MAX];
    uint32_t        spheresLength;
    Color           groundColor;
    uint32_t        skyHeight;
    Color           skyColor;
};

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

// struct Point3_s {
//     // 2D plane (if looking from the top) coordinates.
//     double x;
//     double y;

//     // Height coordinate.
//     double z;
// };


void log_err(char *err);

void init_app(App *app);
void init_screen(App *app);
void init_world(App *app);
// void init_precalc(App *app);
void init_precalc();
void render(App *app);
void render_frame(App *app);
Color ray_trace(Scene *scene, Ray *ray);
bool keyboard_key_pressed();
// float fast_inv_sqrt(float number);

#endif // __MAIN_H__
