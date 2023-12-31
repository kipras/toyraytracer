#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "envconfig.h"
#if defined(ENV_LINUX) && ENV_LINUX
#include "SDL2-2.24.0/include/SDL.h"
#else
#include "SDL2-devel-2.24.0-mingw/x86_64-w64-mingw32/include/SDL2/SDL.h"
#endif // ENV_LINUX

#include "main.h"


// #define DEBUG 1
#define DEBUG 0

bool debug = DEBUG;
#define DEBUG_ANGLE_TO_CC   debug && 1

#define dbg(...) if (debug) { printf(__VA_ARGS__); }

#define assert2(exp, msg) if (! (exp)) { printf("%s", msg); exit(1); }


double piDiv2 = (double)PI / 2;


#if defined(ENV_LINUX) && ENV_LINUX
int main()
#else
int WinMain()
#endif // ENV_LINUX
{
    App *app;

    app = createApp();
    initScreen(app);
    initWorld(app);
    initPrecalc(app);
    render(app);

    return 0;
}

void logErr(char *err)
{
    fprintf(stderr, "%s", err);
}

App * createApp()
{
    App *app;

    setbuf(stdout, NULL);

    app = malloc(sizeof(App));
    if (app == NULL) {
        logErr("Fatal error: could not allocate App. Out of memory?");
        exit(1);
    }

    app->window = NULL;
    app->renderer = NULL;

    return app;
}

void initScreen(App *app)
{
    app->windowWidth    = WINDOW_WIDTH;
    app->windowHeight   = WINDOW_HEIGHT;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(app->windowWidth, app->windowHeight, 0, &app->window, &app->renderer);
    // SDL_RenderSetScale(renderer, 4, 4);

    SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
    SDL_RenderClear(app->renderer);
    SDL_RenderPresent(app->renderer);
}

void initWorld(App *app)
{
    app->camera.from.x = 0;
    app->camera.from.y = 0;
    app->camera.from.z = 0;

    // Camera is looking parallel to the ground (i.e. z=0 (from the range [-1; 1])), in the direction of y=1, x=0 (i.e. "up")
    app->camera.direction.hor  = 0;     // Look in the direction of y=1, x=0 (i.e. "up" on the horizontal plane)
    app->camera.direction.vert = 0;     // Look "sideways" on the vertical plane

    app->fov = FOV;

    app->scene.spheres[0] = (Sphere){.center = {.x = 0, .y = 20, .z = 0}, .radius = 5, .materialType = Matte, .color = COLOR_RED};
}

void initPrecalc(App *app)
{
    Angle      *rwpvac, *rwphac;
    double      deg2radMult;        // Degrees to radians multiplier
    double      fovRadians;         // FOV angle in radians
    double      fovRadiansDiv2;     // FOV angle in radians / 2
    double      vertPixelFovAngle;  // The vertical angle (in radians) covered by the distance of one pixel
    double      horPixelFovAngle;   // The horizontal angle (in radians) covered by the distance of one pixel
    uint32_t    quarter;    // A quarter angle of a 2D plane. Same as a FOV of 90 degrees.
    uint32_t    eighth;     // 1/8 angle of a 2D plane. Same as a FOV of 45 degrees.
    // uint32_t    eighthM1;   // eighth - 1
    uint16_t    whDiv2;     // Window height / 2
    uint16_t    wwDiv2;     // Window width / 2
    // Direction3 *rwpcdc;     // Render window pixels camera direction cache

    whDiv2 = app->windowHeight >> 1;
    wwDiv2 = app->windowWidth >> 1;
    // printf("whDiv2 = %d\n", whDiv2);
    // printf("wwDiv2 = %d\n", wwDiv2);

    deg2radMult = PI / (double)180;
    fovRadians = (double)app->fov * deg2radMult;
    fovRadiansDiv2 = fovRadians / 2;

    vertPixelFovAngle = - fovRadians / (double)app->windowHeight;
    horPixelFovAngle  =   fovRadians / (double)app->windowWidth;

    // Fill renderWindowPixelsVertAngles.
    // First half contains decreasing negative numbers.
    // Second half starts with 0 and contains increasing positive numbers
    rwpvac = app->renderWindowPixelsVertAngles;
    double vertB = whDiv2 * tan(piDiv2 - fovRadiansDiv2);
    // double vertB = app->windowHeight * tan(piDiv2 - fovRadiansDiv2);
    for (int32_t y = -whDiv2; y < whDiv2; y++) {
        // rwpvac[whDiv2 + y] = piDiv2 - atan(vertB / -y);
        rwpvac[whDiv2 + y] = y == 0 ? 0 : atan(-y / vertB);
    }

    // for (int32_t y = -whDiv2; y < whDiv2; y++) {
    //     rwpvac[whDiv2 + y] = piDiv2 - atan(vertB / fabs(y));
    // }

    // for (int32_t y = -whDiv2; y < 0; y++) {
    //     // rwpvac[whDiv2 + y] = piDiv2 - atan(vertB / -y);
    //     rwpvac[whDiv2 + y] = atan(-y / vertB);
    // }
    // rwpvac[whDiv2] = 0;
    // for (int32_t y = 1; y < whDiv2; y++) {
    //     // rwpvac[whDiv2 + y] = - (piDiv2 - atan(vertB / y));
    //     rwpvac[whDiv2 + y] = - atan(y / vertB);
    // }

    // for (int32_t y = -whDiv2; y < whDiv2; y++) {
    //     rwpvac[whDiv2 + y] = y * vertPixelFovAngle;
    // }


    rwphac = app->renderWindowPixelsHorAngles;
    double horB = wwDiv2 * tan(piDiv2 - fovRadiansDiv2);
    // double horB = app->windowWidth * tan(piDiv2 - fovRadiansDiv2);
    for (int32_t x = -wwDiv2; x < wwDiv2; x++) {
        // rwphac[wwDiv2 + x] = - (piDiv2 - atan(horB / -x));
        rwphac[wwDiv2 + x] = x == 0 ? 0 : - atan(-x / horB);
    }

    // for (int32_t x = -wwDiv2; x < wwDiv2; x++) {
    //     rwphac[wwDiv2 + x] = piDiv2 - atan(horB / fabs(x));
    // }

    // for (int32_t x = -wwDiv2; x < 0; x++) {
    //     // rwphac[wwDiv2 + x] = - (piDiv2 - atan(horB / -x));
    //     rwphac[wwDiv2 + x] = - atan(-x / horB);
    // }
    // rwphac[wwDiv2] = 0;
    // for (int32_t x = 1; x < wwDiv2; x++) {
    //     // rwphac[wwDiv2 + x] = piDiv2 - atan(horB / x);
    //     rwphac[wwDiv2 + x] = atan(x / horB);
    // }

    // double vertAnglesSum = 0;
    // for (int32_t y = 0; y < whDiv2; y++) {
    //     vertAnglesSum += rwpvac[y];
    // }
    // printf("vertAnglesSum = %d\n", vertAnglesSum);

    // double horAnglesSum = 0;
    // for (int32_t x = 0; x < wwDiv2; x++) {
    //     horAnglesSum += rwphac[wwDiv2 + x];
    // }
    // printf("horAnglesSum = %d\n", horAnglesSum);
    // exit(1);

    // for (int32_t x = -wwDiv2; x < wwDiv2; x++) {
    //     rwphac[wwDiv2 + x] = x * horPixelFovAngle;
    // }

//    for (int32_t i = 0; i < WINDOW_HEIGHT; i++) {
//        printf("rwpvac[%d] = %f\n", i, rwpvac[i]);
//    }
// //    exit(1);

//    for (int32_t i = 0; i < WINDOW_WIDTH; i++) {
//        printf("rwphac[%d] = %f\n", i, rwphac[i]);
//    }
//    exit(1);


    /*
    // Calculate camera pixel direction angles cache
    eighth = 2 << (ANGLE_BITS - 4); // (2^ANGLE_BITS) / 8
    // eighthM1 = eighth - 1;

    // printf("vertPixelAngle = %d / %d\n", eighth, whDiv2);
    // printf("horPixelAngle = %d / %d\n", eighth, wwDiv2);
    vertPixelAngle = eighth / (double)whDiv2;
    horPixelAngle  = eighth / (double)wwDiv2;
    // printf("vertPixelAngle = %f\n", vertPixelAngle);
    // printf("horPixelAngle = %f\n", horPixelAngle);

    // Fill renderWindowPixelsVertAngles.
    // First half contains decreasing negative numbers.
    // Second half starts with 0 and contains increasing positive numbers
    rwpvac = app->renderWindowPixelsVertAngles;
    for (int32_t y = -whDiv2; y < whDiv2; y++) {
        // Here we assume FOV of 90 degrees, to simplify the calculations (it is simpler, because it matches a quarter of a 2D plane).
        rwpvac[whDiv2 + y] = round(y * vertPixelAngle);
    }

    // for (int32_t i = 0; i < WINDOW_HEIGHT; i++) {
    //     printf("rwpvac[%d] = %d\n", i, rwpvac[i]);
    // }
    // exit(1);

    rwphac = app->renderWindowPixelsHorAngleCache;
    for (int32_t x = -wwDiv2; x < wwDiv2; x++) {
        // Here we assume FOV of 90 degrees, to simplify the calculations (it is simpler, because it matches a quarter of a 2D plane).
        rwphac[wwDiv2 + x] = round(x * horPixelAngle);
    }

    // for (int32_t i = 0; i < WINDOW_WIDTH; i++) {
    //     printf("rwphac[%d] = %d\n", i, rwphac[i]);
    // }
    // exit(1);
     */
}

void render(App *app)
{
    renderFrame(app);

//    // Render 100 rays per pixel
//    for (uint64_t i = 0; i < 100; i++) {
//        renderFrame(app);
//    }

    // SDL_SetRenderDrawColor(app->renderer, 255, 255, 255, 255);
    // SDL_RenderDrawPoint(app->renderer, WINDOW_WIDTH/2, WINDOW_HEIGHT/2);
    // SDL_RenderPresent(app->renderer);

    SDL_Delay(2000);
}

void renderFrame(App *app)
{
    Ray3        ray;
    Color      *color;
    uint32_t    width, height;
    // TraceRes tr;

    width   = app->windowWidth;
    height  = app->windowHeight;

    ray.from = app->camera.from;
//    ray.bounces = 0;

    ray.direction.hor  = 0;
    ray.direction.vert = 0;

    Sphere s[] = {
            (Sphere){.center = {.x = 15, .y = 20, .z = 20}, .radius = 3, .materialType = Matte, .color = COLOR_RED},
            // (Sphere){.center = {.x = 7.7, .y = 20, .z = 0}, .radius = 2, .materialType = Matte, .color = COLOR_RED},

//            (Sphere){.center = {.x = 20, .y = 0, .z = 0}, .radius = 5, .materialType = Matte, .color = COLOR_RED},
//            (Sphere){.center = {.x = 5, .y = 20, .z = 0}, .radius = 5, .materialType = Matte, .color = COLOR_RED},
//            (Sphere){.center = {.x = 3, .y = 20, .z = 0}, .radius = 5, .materialType = Matte, .color = COLOR_RED},
//            (Sphere){.center = {.x = 0, .y = 20, .z = 20}, .radius = 30, .materialType = Matte, .color = COLOR_RED},
//            (Sphere){.center = {.x = 0, .y = 20, .z = 3}, .radius = 5, .materialType = Matte, .color = COLOR_RED},
    };

//    for (uint32_t i = 0; i < sizeof(s) / sizeof(Sphere); i++) {
//        bool intersect = ray3IntersectsSphere(&ray, &s[i]);
//        printf("intersects sphere[%d] = %d\n", i, intersect);
//    }

//    bool intersect = ray3IntersectsSphere(&ray, &app->scene.spheres[0]);
//    printf("intersects sphere = %d\n", intersect);

//    ray.direction.hor  = -0.726493;
//    ray.direction.vert =  1.407172;
//    bool intersect = ray3IntersectsSphere(&ray, &s[0]);
//    printf("intersects sphere = %d\n", intersect);
//    exit(1);

// //    uint32_t y = 0, x = 218;
//     uint32_t y = 320, x = 423;
//     ray.direction.vert = app->renderWindowPixelsVertAngles[y];
//     ray.direction.hor = app->renderWindowPixelsHorAngles[x];
//     bool intersect = ray3IntersectsSphere(&ray, &s[0]);
//     printf("y = %d, x = %d, vert = %f, hor = %f, intersects sphere = %d\n", y, x, ray.direction.vert,
//                ray.direction.hor, intersect);
//     SDL_Delay(2000);
//     exit(1);

    for (uint32_t y = 0; y < height; y++) {
        ray.direction.vert = app->renderWindowPixelsVertAngles[y];
        for (uint32_t x = 0; x < width; x++) {
            ray.direction.hor = app->renderWindowPixelsHorAngles[x];

//            Sphere *sphere = &app->scene.spheres[0];
            Sphere *sphere = &s[0];
            bool intersect = ray3IntersectsSphere(&ray, sphere);

//            printf("y = %d, x = %d, vert = %f, hor = %f, intersects sphere = %d\n", y, x, ray.direction.vert, ray.direction.hor, intersect);
            if (intersect) {
            //    printf("y = %d, x = %d, vert = %f, hor = %f, intersects sphere = %d\n", y, x, ray.direction.vert, ray.direction.hor, intersect);
//                exit(1);

                color = &sphere->color;
                SDL_SetRenderDrawColor(app->renderer, color->red, color->green, color->blue, 255);
                SDL_RenderDrawPoint(app->renderer, x, y);
            }

            // Find an object that we hit
//            color = traceRay(ray);
        }
    }
    SDL_RenderPresent(app->renderer);
}

Color traceRay(Ray3 ray)
{
    // TraceRes res;
}

bool ray3IntersectsSphere(Ray3 *ray3, Sphere *sphere)
{
    bool    intersectHorCircle, intersectVertCircle;
    Ray2    horRay2, vertRay2;
//    Angle   rayAndCCAngleAbs;
    double  zCircleCenterX, zCircleRadius;
    Circle  horCircle, vertCircle;

    horRay2.from.x = ray3->from.x;
    horRay2.from.y = ray3->from.y;
    horRay2.angle  = ray3->direction.hor;

    horCircle.center.x = sphere->center.x;
    horCircle.center.y = sphere->center.y;
    horCircle.radius = sphere->radius;
//    horCircle.radiusDiv2 = sphere->radiusDiv2;

    intersectHorCircle = ray2IntersectsCircle(&horRay2, &horCircle, true, &zCircleCenterX, &zCircleRadius);
    dbg("intersectHorCircle = %d\n", intersectHorCircle);
    if (intersectHorCircle == false) {
        return false;
    }
    dbg("zCircleCenterX = %f\n", zCircleCenterX);
    dbg("zCircleRadius = %f\n", zCircleRadius);

    // @TODO: calculate from camera position
    vertRay2.from.x = 0;
    vertRay2.from.y = 0;

    vertRay2.angle  = ray3->direction.vert;

    vertCircle.center.x = zCircleCenterX;
    vertCircle.center.y = sphere->center.z;
    vertCircle.radius = zCircleRadius;
//    vertCircle.radiusDiv2 = sphere->radiusDiv2;

    dbg("\n");
    dbg("START intersectVertCircle calc\n");
    // if (fabs(horRay2.angle - 0) < 0.001) {
    //     debug = true;
    // }
    intersectVertCircle = ray2IntersectsCircle(&vertRay2, &vertCircle, false, NULL, NULL);
    dbg("intersectVertCircle = %d\n", intersectVertCircle);
    dbg("END intersectVertCircle calc\n");

    return intersectVertCircle;

//    bool intersectHorCircle = false, intersectVertCircle = false;
//
//    intersectHorCircle = ray2IntersectsCircle(&horRay2, &horCircle);
//    dbg("intersectHorCircle = %d\n", intersectHorCircle);
//
//    intersectVertCircle = ray2IntersectsCircle(&vertRay2, &vertCircle);
//    dbg("intersectVertCircle = %d\n", intersectVertCircle);
//
//    return intersectHorCircle && intersectVertCircle;


//    return ray2IntersectsCircle(&horRay2, &horCircle) && ray2IntersectsCircle(&vertRay2, &vertCircle);
}

bool ray2IntersectsCircle(Ray2 *ray2, Circle *circle, bool calculatingHorizontal, double *zCircleCenterX, Angle *zCircleRadius)
{
    Angle  angleToCircleCenter, ccToCircleSideAngle, angleToCircleSide2;
    double radiusDiv2, distToCircleCenter, triangleSideLen;
    Point2 *rayFrom, *cc;

    // // printf("calculatingHorizontal = %d, ray2->angle = %f\n", calculatingHorizontal, ray2->angle);
    // if (debug == true && calculatingHorizontal == false && (fabs(ray2->angle - 1.143736) < 0.001 || fabs(ray2->angle - 0.427060) < 0.001)) {
    //     printf("\nset debug = true\n");
    //     // debug = true;
    // } else {
    //     debug = DEBUG;
    // }

    rayFrom = &ray2->from;
    cc = &circle->center;

    distToCircleCenter = distanceBetweenPoints2(rayFrom, cc);
    dbg("ray2->from = (%f, %f)\n", rayFrom->y, rayFrom->x);
    dbg("ray2->angle = %f\n", ray2->angle);
    dbg("circle->center = (%f, %f)\n", cc->y, cc->x);
    dbg("distToCircleCenter = %f\n", distToCircleCenter);

//    triangleSideLen = distToCircleCenter * sin(fabs(ray2->angle));

    double x = cc->x - rayFrom->x;
    double y = cc->y - rayFrom->y;
    dbg("y = %f, x = %f\n", y, x);
    if (calculatingHorizontal) {
        if (y == 0) {
            if (x == 0) {
                if (DEBUG_ANGLE_TO_CC) { dbg("angleToCircleCenter type: 1.1\n"); }
                return true;
            } else {
                if (DEBUG_ANGLE_TO_CC) { dbg("angleToCircleCenter type: 1.2\n"); }
                angleToCircleCenter = x > 0 ? piDiv2 : -piDiv2;
            }
        } else {
            if (x >= 0) {
                if (y >= 0) {
                    if (DEBUG_ANGLE_TO_CC) { dbg("angleToCircleCenter type: 1.3\n"); }
                    angleToCircleCenter = atan((double)x / y);
                } else {
                    if (DEBUG_ANGLE_TO_CC) { dbg("angleToCircleCenter type: 1.4\n"); }
                    angleToCircleCenter = piDiv2 + atan((double)x / -y);
                }
            } else {
                if (y >= 0) {
                    if (DEBUG_ANGLE_TO_CC) { dbg("angleToCircleCenter type: 1.5\n"); }
                    angleToCircleCenter = -atan((double)-x / y);
                } else {
                    if (DEBUG_ANGLE_TO_CC) { dbg("angleToCircleCenter type: 1.6\n"); }
                    angleToCircleCenter = -(piDiv2 + atan((double)-x / -y));
                }
            }
        }
    } else {
        // calculating vertical
        if (x == 0) {
            if (y == 0) {
                if (DEBUG_ANGLE_TO_CC) { dbg("angleToCircleCenter type: 2.1\n"); }
                return true;
            } else {
                if (DEBUG_ANGLE_TO_CC) { dbg("angleToCircleCenter type: 2.2\n"); }
                angleToCircleCenter = y > 0 ? piDiv2 : -piDiv2;
            }
        } else {
            assert2(x >= 0, "Fatal error: when calculating intersections on vertical plane, x cannot be negative");

            if (y >= 0) {
                if (DEBUG_ANGLE_TO_CC) { dbg("angleToCircleCenter type: 2.3\n"); }
                angleToCircleCenter = atan((double)y / x);
            } else {
                if (DEBUG_ANGLE_TO_CC) { dbg("angleToCircleCenter type: 2.4\n"); }
                angleToCircleCenter = -atan((double)-y / x);
            }
        }
    }

//        radiusDiv2 = circle->radius >> 2;
//    radiusDiv2 = circle->radiusDiv2;
    // radiusDiv2 = circle->radius / 2;
    ccToCircleSideAngle     = asin(circle->radius / distToCircleCenter);
    Angle rayAndCcAngle     = ray2->angle - angleToCircleCenter;
    Angle rayAndCCAngleAbs  = fabs(rayAndCcAngle);

    dbg("ray2->angle = %f\n", ray2->angle);
    dbg("angleToCircleCenter = %f\n", angleToCircleCenter);
    dbg("rayAndCCAngleAbs = %f\n", rayAndCCAngleAbs);
    dbg("ccToCircleSideAngle = %f\n", ccToCircleSideAngle);

//    return rayAndCcAngle <= ccToCircleSideAngle;

    if (calculatingHorizontal) {
        double ccToRayDist = distToCircleCenter * sin(rayAndCCAngleAbs);
        *zCircleRadius = sqrt(circle->radius*circle->radius
                            - ccToRayDist*ccToRayDist);

        *zCircleCenterX = distToCircleCenter * cos(rayAndCCAngleAbs);
//        *rayAndCCAngleAbsRet = rayAndCCAngleAbs;
    }

    return rayAndCCAngleAbs <= ccToCircleSideAngle;



//    return ray2->angle >= angleToCircleCenter - ccToCircleSideAngle
//        && ray2->angle <= angleToCircleCenter + ccToCircleSideAngle;

////    double angleToCircleSide1 = angleToCircleCenter - ccToCircleSideAngle;
//    if (ray2->angle <= angleToCircleCenter - ccToCircleSideAngle) {
//        return false;
//    }
////    angleToCircleSide2 = angleToCircleCenter + ccToCircleSideAngle;
//    if (ray2->angle >= angleToCircleCenter + ccToCircleSideAngle) {
//        return false;
//    }
//    return true;
}

static inline double distanceBetweenPoints2(Point2 *p1, Point2 *p2)
{
    double distX, distY;

    distX = fabs(p1->x - p2->x);
    distY = fabs(p1->y - p2->y);
    return sqrt((double)(distX*distX + distY*distY));
}

float fastInvSqrt(float number)
{
    long i;
    float x2, y;
    const float threehalfs = 1.5F;

    x2 = number * 0.5F;
    y  = number;
    i  = * ( long * ) &y;
    i  = 0x5f3759df - ( i >> 1 );
    y  = * ( float * ) &i;
    y  = y * ( threehalfs - ( x2 * y * y ) );

    return y;
}
