#include <assert.h>
#include <locale.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "main.h"

#include "ray.h"
#include "vector.h"


static inline void output_clear_current_line();
static inline void output_go_up_one_line();


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
    App app;

    init_app(&app);
    init_screen(&app);
    init_world(&app);
    init_precalc(&app);
    render(&app);

    return 0;
}

void log_err(char *err)
{
    fprintf(stderr, "%s", err);
}

void init_app(App *app)
{
    setbuf(stdout, NULL);   // Disable stdout buffering.

    setlocale(LC_NUMERIC, "");  // Set numeric locale, to get printf("%'f") to separate thousands in numbers with commas ","

    app->window = NULL;
    app->renderer = NULL;
}

void init_screen(App *app)
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

void init_world(App *app)
{
    app->camera = (Ray){
        .origin     = (Vector3){.x = 0, .y = 0, .z = 0},
        .direction  = (Vector3){.x = 0, .y = 0, .z = -1},
    };

    // app->camera.origin      = (Vector3){.x = 0, .y = 0, .z = 0};
    // app->camera.direction   = (Vector3){.x = 0, .y = 0, .z = -1};

    // // Camera is looking parallel to the ground (i.e. z=0 (from the range [-1; 1])), in the direction of y=1, x=0 (i.e. "up")
    // app->camera.direction.hor  = 0;     // Look in the direction of y=1, x=0 (i.e. "up" on the horizontal plane)
    // app->camera.direction.vert = 0;     // Look "sideways" on the vertical plane

    // app->fov = FOV;

    // app->scene.spheres[0] = (Sphere){.center = {.x = 0, .y = 0, .z = -10}, .radius = 5, .materialType = Matte, .color = COLOR_RED};
    // app->scene.spheresLength = 1;

    app->scene.spheresLength = 0;
    add_sphere(app, (Sphere){.center = {.x = 0, .y = 0, .z = -15}, .radius = 5, .materialType = Matte, .color = COLOR_RED});        // Center sphere.
    add_sphere(app, (Sphere){.center = {.x = 12, .y = 8.5, .z = -20}, .radius = 5, .materialType = Matte, .color = COLOR_RED});     // Top right sphere.
    add_sphere(app, (Sphere){.center = {.x = -9, .y = 0, .z = -15}, .radius = 2.5, .materialType = Matte, .color = COLOR_RED});     // Center left sphere (small).
    add_sphere(app, (Sphere){.center = {.x = 0, .y = -120, .z = -160}, .radius = 160, .materialType = Matte, .color = COLOR_RED});  // Floor sphere.
}

void add_sphere(App *app, Sphere sphere)
{
    if (app->scene.spheresLength == SCENE_SPHERES_MAX) {
        log_err("Cannot add another sphere to the scene - scene already has maximum spheres");
        exit(1);
    }

    app->scene.spheres[app->scene.spheresLength] = sphere;
    app->scene.spheresLength++;
}

// void init_precalc(App *app)
void init_precalc()
{
}

void render(App *app)
{
    Ray ray = app->camera;

    uint32_t upsampledWindowHeight = app->windowHeight * ANTIALIAS_FACTOR;
    uint32_t upsampledWindowWidth = app->windowWidth * ANTIALIAS_FACTOR;

    uint32_t upsampledWindowMidHeight = upsampledWindowHeight >> 1;
    uint32_t upsampledWindowMidWidth = upsampledWindowWidth >> 1;

    // The initial image into which we will render. This is an upsampled image - it has multiple pixels for each resulting pixel that will
    // be drawn to the screen. We will run antialiasing to produce the final image from this upsampled image.
    Color aaUpsampledImage[upsampledWindowHeight * upsampledWindowWidth];

    Color finalImage[app->windowHeight * app->windowWidth];

    struct timespec tstart, tnow;
    clock_gettime(CLOCK_MONOTONIC, &tstart);

    for (uint64_t frames = 1; ; frames++) {
        // Clear the frame into black.
        SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
        SDL_RenderClear(app->renderer);

        for (uint32_t imgY = 0; imgY < upsampledWindowHeight; imgY++) {
            // Ray direction is from [y=1, x=-1] (top left corner) to [y=-1, x=1] (bottom right corner).
            // Depth (z) remains unchanged at -1.
            ray.direction.y = ((double)(upsampledWindowHeight - imgY) / upsampledWindowMidHeight) - 1;
            // printf("\n");
            // printf("ray.direction.y = %f\n", ray.direction.y);
            uint32_t imgYArrOffset = imgY * upsampledWindowWidth;
            for (uint32_t imgX = 0; imgX < upsampledWindowWidth; imgX++) {
                ray.direction.x = ((double)imgX / upsampledWindowMidWidth) - 1;
                // printf("ray.direction.x = %f\n", ray.direction.x);

                Color color = ray_trace(&app->scene, &ray);
                aaUpsampledImage[imgYArrOffset + imgX] = color;

                // SDL_SetRenderDrawColor(app->renderer, color.red, color.green, color.blue, 255);
                // SDL_RenderDrawPoint(app->renderer, screenX, screenY);

                // bool hit = ray_trace(&app->scene, &ray);
                // if (hit) {
                //     SDL_SetRenderDrawColor(app->renderer, color.red, color.green, color.blue, 255);

                //     // Color *color = &sphere->color;
                //     // SDL_SetRenderDrawColor(app->renderer, color->red, color->green, color->blue, 255);
                //     SDL_RenderDrawPoint(app->renderer, screenX, screenY);
                // }

                // // Draw a two-direction gradient background image.
                // Color color = {
                //     .red = (ray.direction.x + 1) * 128,
                //     .green = (ray.direction.y + 1) * 128,
                //     .blue = 0,
                // };
                // SDL_SetRenderDrawColor(app->renderer, color.red, color.green, color.blue, 255);
                // SDL_RenderDrawPoint(app->renderer, screenX, screenY);
            }
        }

        // Produce the final image, by anti-aliasing the upsampled image.
        image_antialias(aaUpsampledImage, finalImage, app->windowHeight, app->windowWidth, ANTIALIAS_FACTOR);

        // Draw the final image on the screen.
        for (uint32_t screenY = 0; screenY < app->windowHeight; screenY++) {
            uint32_t screenYArrOffset = screenY*app->windowWidth;
            for (uint32_t screenX = 0; screenX < app->windowWidth; screenX++) {
                Color *color = &finalImage[screenYArrOffset + screenX];
                SDL_SetRenderDrawColor(app->renderer, color->red, color->green, color->blue, 255);
                SDL_RenderDrawPoint(app->renderer, screenX, screenY);
            }
        }

        SDL_RenderPresent(app->renderer);

        // Calculate stats.
        clock_gettime(CLOCK_MONOTONIC, &tnow);
        double total_duration = (tnow.tv_sec - tstart.tv_sec) + ((tnow.tv_nsec - tstart.tv_nsec) / 1000000000.0);
        double fps = (double)frames / total_duration;
        double rps = fps * (app->windowHeight*app->windowWidth) * (ANTIALIAS_FACTOR*ANTIALIAS_FACTOR);

        // Output stats (overwriting previous output).
        if (frames > 1) {
            output_go_up_one_line();
            output_clear_current_line();
            output_go_up_one_line();
            output_clear_current_line();
        }
        printf("FPS             = %f\n", fps);
        printf("Rays per second = %'f\n", rps);

        // Run rendering until the user presses any key.
        if (keyboard_key_pressed()) {
            printf("User pressed a key, exiting.\n");
            // SDL_Delay(500);
            return;
        }
    }

    // SDL_SetRenderDrawColor(app->renderer, color->red, color->green, color->blue, 255);
    // SDL_RenderDrawPoint(app->renderer, x, y);
}

void image_antialias(Color *srcImg, Color *dstImg, uint32_t dstHeight, uint32_t dstWidth, uint8_t aaFactor)
{
    uint32_t srcWidth  = dstWidth*aaFactor;
    uint16_t samplesPerPixel = aaFactor*aaFactor;

    uint32_t srcYBase, srcXBase;
    srcYBase = 0;
    for (uint32_t dstY = 0; dstY < dstHeight; dstY++) {
        srcXBase = 0;
        for (uint32_t dstX = 0; dstX < dstWidth; dstX++) {
            // Calculate the antialiased pixel in dstImg from multiple pixels in srcImg.
            uint16_t red = 0, green = 0, blue = 0;
            for (uint32_t srcY = srcYBase; srcY < srcYBase+aaFactor; srcY++) {
                uint32_t srcYArrOffset = srcY*srcWidth;
                for (uint32_t srcX = srcXBase; srcX < srcXBase+aaFactor; srcX++) {
                    Color *srcColor = &srcImg[srcYArrOffset + srcX];
                    red     += srcColor->red;
                    green   += srcColor->green;
                    blue    += srcColor->blue;
                }
            }

            // Calculate the resulting pixel color (averaged from the source pixel colors).
            if (aaFactor == 2) {
                // If antialiasing factor is a power of 2 - then we can use the faster shift-right instead of division.
                dstImg[dstY*dstWidth + dstX] = (Color){.red = red >> 2, .green = green >> 2, .blue = blue >> 2};
            } else {
                dstImg[dstY*dstWidth + dstX] = (Color){.red = red/samplesPerPixel, .green = green/samplesPerPixel, .blue = blue/samplesPerPixel};
            }

            srcXBase += aaFactor;
        }
        srcYBase += aaFactor;
    }
}

bool keyboard_key_pressed()
{
    SDL_Event event;

    // Process all events currently in the event queue.
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_KEYDOWN) {
            return true;
        }
    }
    return false;
}

static inline void output_clear_current_line()
{
    printf("\33[2K\r"); // VT100 escape code for clearing the current output line + LF (carriage return).
}

static inline void output_go_up_one_line()
{
    printf("\033[A");   // VT100 escape code to go one line up in the output.
}

// float fast_inv_sqrt(float number)
// {
//     long i;
//     float x2, y;
//     const float threehalfs = 1.5F;

//     x2 = number * 0.5F;
//     y  = number;
//     i  = * ( long * ) &y;
//     i  = 0x5f3759df - ( i >> 1 );
//     y  = * ( float * ) &i;
//     y  = y * ( threehalfs - ( x2 * y * y ) );

//     return y;
// }
