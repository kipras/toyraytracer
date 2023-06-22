#include <assert.h>
#include <locale.h>

// Need _USE_MATH_DEFINES to have <math.h> export the M_PI constant.
#define _USE_MATH_DEFINES
#include <math.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "main.h"

#include "random.h"
#include "renderer.h"
#include "types.h"
#include "vector.h"


void test_random_point_in_unit_sphere_algorithms_performance();
void test_random_point_in_unit_sphere__random_algo(Vector3 *pArr, uint64_t iterations);
void test_random_point_in_unit_sphere__trigonometric_algo(Vector3 *pArr, uint64_t iterations);

static inline void output_clear_current_line();
static inline void output_go_up_one_line();


double piDiv2 = (double)M_PI / 2;


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

void init_app(App *app)
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

    // app->scene.groundHeight = 0;
    // app->scene.groundColor = (Color)COLOR_GROUND;

    // app->scene.skyHeight = 0;
    // app->scene.skyColor = (Color)COLOR_SKY;

    app->scene.spheresLength = 0;
    add_sphere(app, (Sphere){.center = {.x = 0, .y = 0, .z = -15}, .radius = 5, .material = &matShaded, .color = COLOR_RED});        // Center sphere.
    add_sphere(app, (Sphere){.center = {.x = 12, .y = 8.5, .z = -20}, .radius = 5, .material = &matShaded, .color = COLOR_RED});     // Top right sphere.
    add_sphere(app, (Sphere){.center = {.x = -9, .y = 0, .z = -15}, .radius = 2.5, .material = &matShaded, .color = COLOR_RED});     // Center left sphere (small).

    add_sphere(app, (Sphere){.center = {.x = 0, .y = -1000, .z = -200}, .radius = 1000, .material = &matGround, .color = COLOR_GROUND});  // Ground sphere.

    add_sphere(app, (Sphere){.center = {.x = 0, .y = 0, .z = 0}, .radius = 10000, .material = &matSky, .color = COLOR_SKY});  // Sky sphere.
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

void run_render_loop(App *app)
{
    struct timespec tstart;
    clock_gettime(CLOCK_MONOTONIC, &tstart);

    Color img[app->windowHeight * app->windowWidth];
    for (uint64_t frames = 1; ; frames++) {
        if (ANTIALIAS_FACTOR > 1) {
            render_frame_img_antialiased(app, img, app->windowHeight, app->windowWidth);
        } else {
            render_frame_img(app, img, app->windowHeight, app->windowWidth);
        }
        draw_img_to_screen(app, img, app->windowHeight, app->windowWidth);

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

void output_stats(App *app, struct timespec *tstart, uint64_t frames)
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
