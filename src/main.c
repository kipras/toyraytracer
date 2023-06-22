#include <assert.h>
#include <locale.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "main.h"

#include "random.h"
#include "renderer.h"
#include "rtalloc.h"
#include "types.h"
#include "vector.h"


static void init_app(App *app);
static void init_screen(App *app);
static void init_world(App *app);
static void init_spheres(App *app);
static void add_light(App *app, Sphere sphere, Color32 color);
static void add_sphere(App *app, Sphere sphere);
static void run_render_loop(App *app);
static void output_stats(App *app, struct timespec *tstart, uint64_t frames);
static bool keyboard_key_pressed();
static inline void output_clear_current_line();
static inline void output_go_up_one_line();


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

    run_render_loop(&app);  // The main rendering loop (infinite, until user presses any key).

    return 0;
}

void log_err(char *err)
{
    fprintf(stderr, "%s", err);
}

static void init_app(App *app)
{
    setbuf(stdout, NULL);   // Disable stdout buffering.

    setlocale(LC_NUMERIC, "");  // Set numeric locale, to get printf("%'f") to separate thousands in numbers with commas ","

    // Seed the random number generator.
    struct timespec tnow;
    clock_gettime(CLOCK_MONOTONIC, &tnow);
    random_seed(tnow.tv_nsec);

    app->window = NULL;
    app->renderer = NULL;
}

static void init_screen(App *app)
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

static void init_world(App *app)
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

    // app->scene.groundHeight = 0;
    // app->scene.groundColor = (Color)COLOR_GROUND;

    // app->scene.skyHeight = 0;
    // app->scene.skyColor = (Color)COLOR_SKY;

    init_spheres(app);
}

static void init_spheres(App *app)
{
    app->scene.spheresLength = 0;

    // add_sphere(app, (Sphere){.center = {.x = 0, .y = 0, .z = -30}, .radius = 10, .material = &matMatte, .color = COLOR_RED});        // Center sphere.
    add_sphere(app, (Sphere){.center = {.x = 24, .y = 17, .z = -40}, .radius = 10, .material = &matMatte, .color = COLOR_GREEN});    // Top right sphere.
    add_sphere(app, (Sphere){.center = {.x = -18, .y = -4, .z = -30}, .radius = 5, .material = &matMatte, .color = COLOR_BLUE});     // Center left sphere (small).

    add_sphere(app, (Sphere){.center = {.x = 0, .y = 0, .z = -30}, .radius = 10, .material = &matMatte, .color = COLOR_RED});       // Center sphere (the big one).

    add_sphere(app, (Sphere){.center = {.x = -8, .y = -8, .z = -12.5}, .radius = 3, .material = &matMatte, .color = COLOR_BLUE});   // Bottom left sphere (small).
    add_sphere(app, (Sphere){.center = {.x = 0, .y = -8, .z = -12.5}, .radius = 3, .material = &matMatte, .color = COLOR_RED});     // Bottom center sphere (small).
    add_sphere(app, (Sphere){.center = {.x = 8, .y = -8, .z = -12.5}, .radius = 3, .material = &matMatte, .color = COLOR_GREEN});   // Bottom right sphere (small).

    add_light(app, (Sphere){.center = {.x = -9, .y = 10, .z = -20}, .radius = 3, .material = &matLight}, (Color32)COLOR32_LIGHT);    // A light.

    // add_sphere(app, (Sphere){.center = {.x = 0, .y = -2000, .z = -400}, .radius = 2000, .material = &matGround, .color = COLOR_GROUND});  // Ground sphere.
    add_sphere(app, (Sphere){.center = {.x = 0, .y = -2000, .z = -220}, .radius = 2000, .material = &matMatte, .color = COLOR_GROUND});  // Ground sphere.

    // add_sphere(app, (Sphere){.center = {.x = 0, .y = 0, .z = 0}, .radius = 20000, .material = &matGradientSky, .color = COLOR_BLACK});  // Sky sphere (COLOR_BLACK is the Color equivalent of NULL).

    // add_light(app, (Sphere){.center = {.x = 0, .y = 0, .z = 0}, .radius = 20000, .material = &matLight}, (Color32)COLOR_SKY);      // Sky sphere, providing an ambient light.

    add_light(app, (Sphere){.center = {.x = 0, .y = 0, .z = 0}, .radius = 20000, .material = &matLight}, (Color32)COLOR_AMBIENT_LIGHT);      // Sky sphere, providing an ambient light.
}

static void add_light(App *app, Sphere sphere, Color32 color)
{
    MaterialLightData *matData = ralloc(sizeof(Color32));
    matData->color = color;
    sphere.matData = matData;

    add_sphere(app, sphere);
}

static void add_sphere(App *app, Sphere sphere)
{
    if (app->scene.spheresLength == SCENE_SPHERES_MAX) {
        log_err("Cannot add another sphere to the scene - scene already has maximum spheres");
        exit(1);
    }

    app->scene.spheres[app->scene.spheresLength] = sphere;
    app->scene.spheresLength++;
}

static void run_render_loop(App *app)
{
    struct timespec tstart;
    clock_gettime(CLOCK_MONOTONIC, &tstart);

    Color32 allFrames[app->windowHeight * app->windowWidth];        // NOTE: should this perhaps be Color64 (to avoid overflow/cap) ?
    Color32 frameImg[app->windowHeight * app->windowWidth];
    Color blendedImg[app->windowHeight * app->windowWidth];
    for (uint32_t frames = 1; ; frames++) {
        if (ANTIALIAS_FACTOR > 1) {
            render_frame_img_antialiased(app, frameImg, app->windowHeight, app->windowWidth);
        } else {
            render_frame_img(app, frameImg, app->windowHeight, app->windowWidth);
        }

        // draw_img_to_screen(app, frameImg, app->windowHeight, app->windowWidth);

        blend_frame(allFrames, frames, frameImg, blendedImg, app->windowHeight, app->windowWidth);
        draw_img_to_screen(app, blendedImg, app->windowHeight, app->windowWidth);

        // Calculate & output performance stats
        output_stats(app, &tstart, frames);

        // Run rendering until the user presses any key.
        if (keyboard_key_pressed()) {
            printf("User pressed a key, exiting.\n");
            // SDL_Delay(500);
            return;
        }
    }
}

static void output_stats(App *app, struct timespec *tstart, uint64_t frames)
{
    struct timespec tnow;

    // Calculate stats.
    clock_gettime(CLOCK_MONOTONIC, &tnow);
    double total_duration = (tnow.tv_sec - tstart->tv_sec) + ((tnow.tv_nsec - tstart->tv_nsec) / 1000000000.0);
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
}

static bool keyboard_key_pressed()
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
