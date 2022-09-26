#include <math.h>
#include <stdio.h>
// #include <stdlib.h>

#include "SDL2-2.24.0/x86_64-w64-mingw32/include/SDL2/SDL.h"

#include "main.h"


int WinMain()
{
    App *app;

    app = createApp();
    initScreen(app);
    initWorld(app);
    render(app);

    return 0;
}

void logErr(char *err)
{
    fprintf(stderr, err);
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
    Angle      *rwpvac, *rwphac;
    double      vertPixelAngle;     // The vertical angle covered by the distance of one pixel
    double      horPixelAngle;      // The horizontal angle covered by the distance of one pixel
    uint32_t    quarter;    // A quarter angle of a 2D plane. Same as a FOV of 90 degrees.
    uint32_t    eighth;     // 1/8 angle of a 2D plane. Same as a FOV of 45 degrees.
    // uint32_t    eighthM1;   // eighth - 1
    uint16_t    whDiv2;     // Window height / 2
    uint16_t    wwDiv2;     // Window width / 2
    // Direction3 *rwpcdc;     // Render window pixels camera direction cache
    Direction  *camDir;

    app->camera.from.x = 0;
    app->camera.from.y = 0;
    app->camera.from.z = 100;

    // Camera is looking parallel to the ground (i.e. z=0 (from the range [-1; 1])), in the direction of y=1, x=0 (i.e. "up")
    app->camera.direction.hor  = 0;     // Look in the direction of y=1, x=0 (i.e. "up" on the horizontal plane)
    app->camera.direction.vert = 0;     // Look "sideways" on the vertical plane
    camDir = &app->camera.direction;

    app->fov = FOV;

    // Calculate camera pixel directions cache
    // rwpcdc = (Direction3 *) &app->renderWindowPixelsCameraDirectionCache;
    eighth = 2 << (ANGLE_BITS - 4); // (2^ANGLE_BITS) / 8
    // eighthM1 = eighth - 1;

    rwpvac = app->renderWindowPixelsVertAngleCache;
    rwphac = app->renderWindowPixelsHorAngleCache;

    whDiv2 = app->windowHeight >> 1;
    wwDiv2 = app->windowWidth >> 1;
    // printf("whDiv2 = %d\n", whDiv2);
    // printf("wwDiv2 = %d\n", wwDiv2);

    // printf("vertPixelAngle = %d / %d\n", eighth, whDiv2);
    // printf("horPixelAngle = %d / %d\n", eighth, wwDiv2);
    vertPixelAngle = eighth / (double)whDiv2;
    horPixelAngle  = eighth / (double)wwDiv2;
    // printf("vertPixelAngle = %f\n", vertPixelAngle);
    // printf("horPixelAngle = %f\n", horPixelAngle);

    // Fill renderWindowPixelsVertAngleCache.
    // First half contains decreasing negative numbers.
    // Second half starts with 0 and contains increasing positive numbers.
    for (int32_t y = -whDiv2; y < whDiv2; y++) {
        // Here we assume FOV of 90 degrees, to simplify the calculations (it is simpler, because it matches a quarter of a 2D plane).
        rwpvac[whDiv2 + y] = round(y * vertPixelAngle);
    }

    // for (int32_t i = 0; i < WINDOW_HEIGHT; i++) {
    //     printf("rwpvac[%d] = %d\n", i, rwpvac[i]);
    // }
    // exit(1);

    for (int32_t x = -wwDiv2; x < wwDiv2; x++) {
        // Here we assume FOV of 90 degrees, to simplify the calculations (it is simpler, because it matches a quarter of a 2D plane).
        rwphac[wwDiv2 + x] = round(x * horPixelAngle);
    }

    // for (int32_t i = 0; i < WINDOW_WIDTH; i++) {
    //     printf("rwphac[%d] = %d\n", i, rwphac[i]);
    // }
    // exit(1);
}

void render(App *app)
{
    // Render 100 rays per pixel
    for (uint64_t i = 0; i < 100; i++) {
        renderFrame(app);
    }

    // SDL_SetRenderDrawColor(app->renderer, 255, 255, 255, 255);
    // SDL_RenderDrawPoint(app->renderer, 640/2, 480/2);
    // SDL_RenderPresent(app->renderer);

    SDL_Delay(2000);
}

void renderFrame(App *app)
{
    Ray      ray;
    Color    color;
    uint32_t width, height;
    // TraceRes tr;

    width   = app->windowWidth;
    height  = app->windowHeight;

    ray.df.from = app->camera.from;
    ray.bounces = 0;
    for (uint32_t y = 0; y < height; y++) {
        for (uint32_t x = 0; x < width; x++) {
            // Find an object that we hit


            color = traceRay(ray);
        }
    }
}

static Color traceRay(Ray ray)
{
    // TraceRes res;
}
