#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdbool.h>

#include "envconfig.h"
#if defined(ENV_LINUX) && ENV_LINUX
#include "SDL2-2.24.0/include/SDL.h"
#else
#include "SDL2-devel-2.24.0-mingw/x86_64-w64-mingw32/include/SDL2/SDL.h"
#endif // ENV_LINUX

#define PI                  3.14159265

#define WINDOW_WIDTH        640
#define WINDOW_HEIGHT       640
#define FOV                 90
#define SCENE_SPHERES_MAX   20
#define RAY_BOUNCES_MAX     20


typedef double                  Angle;  // Angles are stored as radians
//#define ANGLE_BITS              32

typedef struct Point2_s         Point2;
typedef struct Ray2_s           Ray2;
typedef struct Point3_s         Point3;
typedef struct Direction3_s     Direction3;
typedef struct Ray3_s           Ray3;
//typedef struct Ray3_s           Ray3;
typedef struct Color_s          Color;
typedef enum   MaterialType_e   MaterialType;
typedef enum   ObjectType_e     ObjectType;
typedef struct Circle_s         Circle;
typedef struct Sphere_s         Sphere;
typedef struct Scene_s          Scene;
typedef enum   TraceHitType_e   TraceHitType;
typedef struct TraceRes_s       TraceRes;
typedef struct App_s            App;


struct Point2_s {
    double  x;
    double  y;
};

struct Ray2_s {
    Point2  from;
    Angle   angle;
};

struct Point3_s {
    double  x;
    double  y;
    double  z;
};

struct Direction3_s {
    // Angle on a horizontal plane (a full circle) as a 32 bit signed integer:
    // 0 means:     y=1,  x=0;  i.e. "up" (if looking at the plane)
    // 2^30 means:  y=0,  x=1;  i.e. "right" (if looking at the plane)
    // -2^30 means: y=0,  x=-1; i.e. "left" (if looking at the plane)
    // -2^31 means: y=-1, x=0;  i.e. "down" (if looking at the plane)
    Angle   hor;

    // Angle on a vertical plane (a half-circle) as a 32 bit signed integer:
    // -2^30 means: y=1,  x=0;  i.e. "up" (if looking at the plane)
    // 0 means:     y=0,  x=1;  i.e. "sideways" (if looking at the plane)
    // 2^31 means:  y=-1, x=0;  i.e. "down" (if looking at the plane)
    //
    // Note that [2^31+1, 2^32-1] are never used, because this plane is a half-circle.
    Angle   vert;
};

struct Ray3_s {
    Point3      from;
    Direction3  direction;
};

//struct Ray3_s {
//    Ray3  df;
//    uint8_t         bounces;    // How many bounces this ray already had
//};

struct Color_s {
    uint8_t     red;
    uint8_t     green;
    uint8_t     blue;
};
#define COLOR_RED   {255,   0,   0}
#define COLOR_GREEN {  0, 255,   0}
#define COLOR_BLUE  {  0,   0, 255}


enum MaterialType_e {
    Matte = 1, Metal, Glass,
};

enum ObjectType_e {
    OTSphere = 1,
};

struct Circle_s {
    Point2      center;
    double      radius;
//    double      radiusDiv2;
};

struct Sphere_s {
    Point3          center;
    uint32_t        radius;
//    double          radiusDiv2;
    MaterialType    materialType;
    Color           color;
};

struct Scene_s {
    Sphere          spheres[SCENE_SPHERES_MAX];
    Color           groundColor;
    uint32_t        skyHeight;
    Color           skyColor;
};

enum TraceHitType_e {
    Object = 1, Ground = 2, Sky = 3,
};

// struct TraceRes_s {
//     TraceHitType    hitType;
//     ObjectType      objectType;
//     Sphere          *sphere;
//     DirectionFrom    bounce;
// };

struct App_s {
    SDL_Window     *window;
    SDL_Renderer   *renderer;

    uint32_t        windowWidth;
    uint32_t        windowHeight;

    Scene           scene;
    Ray3            camera;

    // Precalculated camera angles for screen pixels
    Angle           renderWindowPixelsHorAngles[WINDOW_WIDTH];
    Angle           renderWindowPixelsVertAngles[WINDOW_HEIGHT];

    // Direction       renderWindowPixelsCameraDirectionCache[WINDOW_HEIGHT * WINDOW_WIDTH];

    // FOV can theoretically go up to the full 360 degrees. However, practically it usually goes up to no more than
    // 120 degrees in graphics applications, so we use an 8 bit integer (up to 255) to store it.
    uint8_t         fov;
};


void logErr(char *err);
App * createApp();
void initScreen(App *app);
void initWorld(App *app);
void initPrecalc(App *app);
void render(App *app);
void renderFrame(App *app);
Color traceRay(Ray3 ray);
bool ray3IntersectsSphere(Ray3 *ray3, Sphere *sphere);
bool ray2IntersectsCircle(Ray2 *ray2, Circle *circle, bool calculatingHorizontal, double *zCircleCenterX, Angle *rayAndCCAngleAbsRet);
static inline double distanceBetweenPoints2(Point2 *p1, Point2 *p2);
float fastInvSqrt(float number);

#endif // __MAIN_H__
