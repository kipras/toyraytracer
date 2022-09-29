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
}

void initScreen(App *app)
{
    app->windowWidth    = 640;
    app->windowHeight   = 480;

    SDL_Init(SDL_INIT_VIDEO);
    // SDL_CreateWindowAndRenderer(640*4, 480*4, 0, &window, &renderer);
    // SDL_RenderSetScale(renderer, 4, 4);
    SDL_CreateWindowAndRenderer(app->windowWidth, app->windowHeight, 0, &app->window, &app->renderer);

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

    app->scene.spheres[0] = (Sphere){.center = {.x = 0, .y = 20, .z = 0}, .radius = 5, .radiusDiv2 = 2.5, .materialType = Matte, .color = COLOR_RED};
}

void initPrecalc(App *app)
{
    Angle      *rwpvac, *rwphac;
    double      deg2radMult;        // Degrees to radians multiplier
    double      fovRadians;         // FOV angle in radians
    double      vertPixelAngle;     // The vertical angle (in radians) covered by the distance of one pixel
    double      horPixelAngle;      // The horizontal angle (in radians) covered by the distance of one pixel
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

    vertPixelAngle = - fovRadians / (double)whDiv2;
    horPixelAngle  =   fovRadians / (double)wwDiv2;

    // Fill renderWindowPixelsVertAngles.
    // First half contains decreasing negative numbers.
    // Second half starts with 0 and contains increasing positive numbers
    rwpvac = app->renderWindowPixelsVertAngles;
    for (int32_t y = -whDiv2; y < whDiv2; y++) {
        // Here we assume FOV of 90 degrees, to simplify the calculations (it is simpler, because it matches a quarter of a 2D plane).
        rwpvac[whDiv2 + y] = y * vertPixelAngle;
    }

//    for (int32_t i = 0; i < WINDOW_HEIGHT; i++) {
//        printf("rwpvac[%d] = %f\n", i, rwpvac[i]);
//    }
//    exit(1);

    rwphac = app->renderWindowPixelsHorAngles;
    for (int32_t x = -wwDiv2; x < wwDiv2; x++) {
        // Here we assume FOV of 90 degrees, to simplify the calculations (it is simpler, because it matches a quarter of a 2D plane).
        rwphac[wwDiv2 + x] = x * horPixelAngle;
    }

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
    // SDL_RenderDrawPoint(app->renderer, 640/2, 480/2);
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
            (Sphere){.center = {.x = 25, .y = 20, .z = -15}, .radius = 5, .radiusDiv2 = 2.5, .materialType = Matte, .color = COLOR_RED},
//            (Sphere){.center = {.x = 20, .y = 0, .z = 0}, .radius = 5, .radiusDiv2 = 2.5, .materialType = Matte, .color = COLOR_RED},
//            (Sphere){.center = {.x = 5, .y = 20, .z = 0}, .radius = 5, .radiusDiv2 = 2.5, .materialType = Matte, .color = COLOR_RED},
//            (Sphere){.center = {.x = 3, .y = 20, .z = 0}, .radius = 5, .radiusDiv2 = 2.5, .materialType = Matte, .color = COLOR_RED},
//            (Sphere){.center = {.x = 0, .y = 20, .z = 20}, .radius = 30, .radiusDiv2 = 15, .materialType = Matte, .color = COLOR_RED},
//            (Sphere){.center = {.x = 0, .y = 20, .z = 3}, .radius = 5, .radiusDiv2 = 2.5, .materialType = Matte, .color = COLOR_RED},
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

    for (uint32_t y = 0; y < height; y++) {
        ray.direction.vert = app->renderWindowPixelsVertAngles[y];
        for (uint32_t x = 0; x < width; x++) {
            ray.direction.hor = app->renderWindowPixelsHorAngles[x];

//            Sphere *sphere = &app->scene.spheres[0];
            Sphere *sphere = &s[0];
            bool intersect = ray3IntersectsSphere(&ray, sphere);
//            printf("y = %d, x = %d, vert = %f, hor = %f, intersects sphere = %d\n", y, x, ray.direction.vert, ray.direction.hor, intersect);
            if (intersect) {
//                printf("y = %d, x = %d, vert = %f, hor = %f, intersects sphere = %d\n", y, x, ray.direction.vert, ray.direction.hor, intersect);
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
    Ray2    horRay2, vertRay2;
    Circle  horCircle, vertCircle;

    horRay2.from.x = ray3->from.x;
    horRay2.from.y = ray3->from.y;
    horRay2.angle  = ray3->direction.hor;

    horCircle.center.x = sphere->center.x;
    horCircle.center.y = sphere->center.y;
    horCircle.radius = sphere->radius;
    horCircle.radiusDiv2 = sphere->radiusDiv2;

    vertRay2.from.x = ray3->from.z;
    vertRay2.from.y = ray3->from.y;
    vertRay2.angle  = ray3->direction.vert;

    vertCircle.center.x = sphere->center.z;
    vertCircle.center.y = sphere->center.y;
    vertCircle.radius = sphere->radius;
    vertCircle.radiusDiv2 = sphere->radiusDiv2;

//    bool intersectHorCircle = false, intersectVertCircle = false;

//    intersectHorCircle = ray2IntersectsCircle(&horRay2, &horCircle);
//    printf("intersectHorCircle = %d\n", intersectHorCircle);

//    intersectVertCircle = ray2IntersectsCircle(&vertRay2, &vertCircle);
//    printf("intersectVertCircle = %d\n", intersectVertCircle);

//    return intersectHorCircle && intersectVertCircle;

    return ray2IntersectsCircle(&horRay2, &horCircle) && ray2IntersectsCircle(&vertRay2, &vertCircle);
}

bool ray2IntersectsCircle(Ray2 *ray2, Circle *circle)
{
    double radiusDiv2, angleToCircleCenter, angleToSideDiff, angleToCircleSide2;
    double distToCircleCenter, triangleSideLen;
    Point2 *rayFrom, *cc;

    rayFrom = &ray2->from;
    cc = &circle->center;

    distToCircleCenter = distanceBetweenPoints2(rayFrom, cc);
//    printf("ray2->from = (%d, %d)\n", rayFrom->y, rayFrom->x);
//    printf("circle->center = (%d, %d)\n", cc->y, cc->x);
//    printf("distToCircleCenter = %f\n", distToCircleCenter);

//    triangleSideLen = distToCircleCenter * sin(fabs(ray2->angle));

    int32_t x = cc->x - rayFrom->x;
    int32_t y = cc->y - rayFrom->y;
    if (y == 0) {
        if (x == 0) {
            return true;
        } else {
            angleToCircleCenter = x > 0 ? piDiv2 : -piDiv2;
        }
    } else {
        if (x >= 0) {
            if (y >= 0) {
                angleToCircleCenter = atan((double)x / y);
            } else {
                angleToCircleCenter = piDiv2 + atan((double)x / -y);
            }
        } else {
            if (y >= 0) {
                angleToCircleCenter = -atan((double)-x / y);
            } else {
                angleToCircleCenter = -(piDiv2 + atan((double)-x / -y));
            }
        }
    }

    //    radiusDiv2 = circle->radius >> 2;
    radiusDiv2 = circle->radiusDiv2;
    angleToSideDiff = 2 * (asin(radiusDiv2 / distToCircleCenter));

//    printf("angleToCircleCenter = %f\n", angleToCircleCenter);
//    printf("angleToSideDiff = %f\n", angleToSideDiff);
//    printf("ray2->angle = %f\n", ray2->angle);

//    double angleToCircleSide1 = angleToCircleCenter - angleToSideDiff;
    if (ray2->angle <= angleToCircleCenter - angleToSideDiff) {
        return false;
    }
//    angleToCircleSide2 = angleToCircleCenter + angleToSideDiff;
    if (ray2->angle >= angleToCircleCenter + angleToSideDiff) {
        return false;
    }
    return true;
}

double distanceBetweenPoints2(Point2 *p1, Point2 *p2)
{
    uint32_t distX, distY;

    distX = abs(p1->x - p2->x);
    distY = abs(p1->y - p2->y);
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
