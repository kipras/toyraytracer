#include <assert.h>
#include <locale.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "main.h"

#include "camera.h"
#include "random.h"
#include "renderer.h"
#include "rtalloc.h"
#include "types.h"
#include "vector.h"


static void init_app(App *app);
static void init_screen(App *app);
static void init_world(App *app);
static void init_scene(App *app);
static void add_light(App *app, Sphere sphere, Color color);
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
    // Our camera is centered at [0, 0, 0] and looking towards the y axis.
    Vector3 camDirection = {.x = 0, .y = 1, .z = 0};
    vector3_to_unit(&camDirection);     // camera direction vector must
    Ray camCenterRay = {
        .origin     = (Vector3){.x = 0, .y = 0, .z = 0},
        .direction  = camDirection,
    };

    cam_set(&app->camera, &camCenterRay, app->windowHeight * ANTIALIAS_FACTOR, app->windowWidth * ANTIALIAS_FACTOR);

    init_scene(app);
}

static void init_scene(App *app)
{
    app->scene.spheresLength = 0;

    // Choose one of the available scenes (descriptions inside each function).
    SceneConfig sc = SC_6_spheres_fov_90;
    switch (sc) {
        case SC_none:
            break;

        case SC_6_spheres_fov_90:
            scene_6_spheres_fov_90(app); break;

        case SC_6_spheres_fov_40:
            scene_6_spheres_fov_40(app); break;

        case SC_camera_testing_1_sphere_fov_90:
            scene_camera_testing_1_sphere_fov_90(app); break;

        case SC_camera_testing_4_spheres_fov_90:
            scene_camera_testing_4_spheres_fov_90(app); break;

        case SC_camera_testing_4_spheres_fov_40:
            scene_camera_testing_4_spheres_fov_40(app); break;
    }

    // Choose one of the available sky configurations (descriptions inside each function).
    SkyConfig sk = SK_gradient_blue;
    switch (sk) {
        case SK_none:
            break;

        case SK_ambient_07:
            sky_ambient_07(app); break;

        case SK_gradient_blue:
            sky_gradient_blue(app); break;

        case SK_ambient_blue:
            sky_ambient_blue(app); break;
    }
}

void scene_6_spheres_fov_90(App *app)
{
    // Standard 6 sphere scene. FOV 90.

    // add_sphere(app, (Sphere){.center = {.x = 0, .y = 30, .z = 0}, .radius = 10, .material = &matMatte, .color = COLOR_RED});        // Center sphere.
    add_sphere(app, (Sphere){.center = {.x = 24, .y = 40, .z = 17}, .radius = 10, .material = &matMatte, .color = COLOR_GREEN});    // Top right sphere.
    add_sphere(app, (Sphere){.center = {.x = -18, .y = 30, .z = -4}, .radius = 5, .material = &matMatte, .color = COLOR_BLUE});     // Center left sphere (small).

    add_sphere(app, (Sphere){.center = {.x = 0, .y = 30, .z = 0}, .radius = 10, .material = &matMatte, .color = COLOR_RED});       // Center sphere (the big one).

    add_sphere(app, (Sphere){.center = {.x = -8, .y = 12.5, .z = -8}, .radius = 3, .material = &matMatte, .color = COLOR_BLUE});   // Bottom left sphere (small).
    add_sphere(app, (Sphere){.center = {.x = 0, .y = 12.5, .z = -8}, .radius = 3, .material = &matMatte, .color = COLOR_RED});     // Bottom center sphere (small).
    add_sphere(app, (Sphere){.center = {.x = 8, .y = 12.5, .z = -8}, .radius = 3, .material = &matMatte, .color = COLOR_GREEN});   // Bottom right sphere (small).

    add_light(app, (Sphere){.center = {.x = -9, .y = 20, .z = 10}, .radius = 3, .material = &matLight}, (Color)COLOR_LIGHT);    // A light.

    add_sphere(app, (Sphere){.center = {.x = 0, .y = 220, .z = -2000}, .radius = 2000, .material = &matMatte, .color = COLOR_GROUND});  // Ground sphere.
}

void scene_6_spheres_fov_40(App *app)
{
    // Standard 6 sphere scene. FOV 40.

    // add_sphere(app, (Sphere){.center = {.x = 0, .y = 30, .z = 0}, .radius = 10, .material = &matMatte, .color = COLOR_RED});        // Center sphere.
    add_sphere(app, (Sphere){.center = {.x = 24, .y = 40, .z = 17}, .radius = 10, .material = &matMatte, .color = COLOR_GREEN});    // Top right sphere.
    add_sphere(app, (Sphere){.center = {.x = -18, .y = 30, .z = -4}, .radius = 5, .material = &matMatte, .color = COLOR_BLUE});     // Center left sphere (small).

    add_sphere(app, (Sphere){.center = {.x = 0, .y = 30, .z = 0}, .radius = 10, .material = &matMatte, .color = COLOR_RED});       // Center sphere (the big one).

    add_sphere(app, (Sphere){.center = {.x = -8, .y = 12.5, .z = -8}, .radius = 3, .material = &matMatte, .color = COLOR_BLUE});   // Bottom left sphere (small).
    add_sphere(app, (Sphere){.center = {.x = 0, .y = 12.5, .z = -8}, .radius = 3, .material = &matMatte, .color = COLOR_RED});     // Bottom center sphere (small).
    add_sphere(app, (Sphere){.center = {.x = 8, .y = 12.5, .z = -8}, .radius = 3, .material = &matMatte, .color = COLOR_GREEN});   // Bottom right sphere (small).

    add_light(app, (Sphere){.center = {.x = -9, .y = 20, .z = 10}, .radius = 3, .material = &matLight}, (Color)COLOR_LIGHT);    // A light.

    add_sphere(app, (Sphere){.center = {.x = 0, .y = 220, .z = -2000}, .radius = 2000, .material = &matMatte, .color = COLOR_GROUND});  // Ground sphere.
}

void scene_camera_testing_1_sphere_fov_90(App *app)
{
    // CAMERA TESTING SETUP: single big center sphere.
    // For rectangular image resolution only. For FOV 90.
    add_sphere(app, (Sphere){.center = {.x = 0, .y = 1.4142135623730950488016887242097, .z = 0}, .radius = 1, .material = &matMatte, .color = COLOR_RED});
}

void scene_camera_testing_4_spheres_fov_90(App *app)
{
    // CAMERA TESTING SETUP: 4 spheres at top/right/bottom/left, diameter of each is 1/4 of screen width/height (i.e. they do not touch
    // each other).
    // For rectangular image resolution only. For FOV 90.
    add_sphere(app, (Sphere){.center = {.x = 0, .y = 3.5355339059327376220042218105242, .z = 2.1213203435596425732025330863145}, .radius = 1, .material = &matMatte, .color = COLOR_RED});  // top
    add_sphere(app, (Sphere){.center = {.x = -2.1213203435596425732025330863145, .y = 3.5355339059327376220042218105242, .z = 0}, .radius = 1, .material = &matMatte, .color = COLOR_RED}); // left
    add_sphere(app, (Sphere){.center = {.x = 2.1213203435596425732025330863145, .y = 3.5355339059327376220042218105242, .z = 0}, .radius = 1, .material = &matMatte, .color = COLOR_RED});  // right
    add_sphere(app, (Sphere){.center = {.x = 0, .y = 3.5355339059327376220042218105242, .z = -2.1213203435596425732025330863145}, .radius = 1, .material = &matMatte, .color = COLOR_RED}); // bottom
}

void scene_camera_testing_4_spheres_fov_40(App *app)
{
    // CAMERA TESTING SETUP: 4 spheres at top/right/bottom/left, diameter of each is 1/4 of screen width/height (i.e. they do not touch
    // each other).
    // For rectangular image resolution only. For FOV 40.
    add_sphere(app, (Sphere){.center = {.x = 0, .y = 10.669157170675342809798718809418, .z = 2.8190778623577251521623278319742}, .radius = 1, .material = &matMatte, .color = COLOR_RED});  // top
    add_sphere(app, (Sphere){.center = {.x = -2.8190778623577251521623278319742, .y = 10.669157170675342809798718809418, .z = 0}, .radius = 1, .material = &matMatte, .color = COLOR_RED}); // left
    add_sphere(app, (Sphere){.center = {.x = 2.8190778623577251521623278319742, .y = 10.669157170675342809798718809418, .z = 0}, .radius = 1, .material = &matMatte, .color = COLOR_RED});  // right
    add_sphere(app, (Sphere){.center = {.x = 0, .y = 10.669157170675342809798718809418, .z = -2.8190778623577251521623278319742}, .radius = 1, .material = &matMatte, .color = COLOR_RED}); // bottom
}

void sky_ambient_07(App *app)
{
    // Sky sphere, providing an ambient light (COLOR_BLACK is the Color equivalent of NULL).
    add_light(app, (Sphere){.center = {.x = 0, .y = 0, .z = 0}, .radius = 20000, .material = &matLight}, (Color)COLOR_AMBIENT_LIGHT);
}

void sky_gradient_blue(App *app)
{
    // Sky sphere, providing a gradient (blue-white) light.
    add_sphere(app, (Sphere){.center = {.x = 0, .y = 0, .z = 0}, .radius = 20000, .material = &matGradientSky, .color = COLOR_BLACK});
}

void sky_ambient_blue(App *app)
{
    // Sky sphere, providing a ambient blue-ish light.
    add_light(app, (Sphere){.center = {.x = 0, .y = 0, .z = 0}, .radius = 20000, .material = &matLight}, (Color)COLOR_SKY);
}

static void add_light(App *app, Sphere sphere, Color color)
{
    MaterialLightData *matData = rtalloc(sizeof(Color));
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

    Color allFrames[app->windowHeight * app->windowWidth];        // NOTE: should this perhaps be Color64 (to avoid overflow/cap) ?

    // Have to clear the allFrames buffer first. The other buffers we don't need to clear, because we will be directly writing images to
    // them. But the allFrames buffer will be _appended to_ instead of _set_, so we must make sure that each color of each pixel starts
    // from 0
    memset(&allFrames, 0, sizeof(Color) * app->windowHeight * app->windowWidth);

    Color frameImg[app->windowHeight * app->windowWidth];
    Color blendedImg[app->windowHeight * app->windowWidth];
    for (uint32_t frames = 1; ; frames++) {
        if (ANTIALIAS_FACTOR > 1) {
            render_frame_img_antialiased(app, frameImg, app->windowHeight, app->windowWidth);
        } else {
            render_frame_img(app, frameImg, app->windowHeight, app->windowWidth);
        }

        // (void)blendedImg;
        // draw_img_to_screen(app, frameImg, app->windowHeight, app->windowWidth);

        blend_frame(allFrames, frames, frameImg, blendedImg, app->windowHeight, app->windowWidth);
        draw_img_to_screen(app, blendedImg, app->windowHeight, app->windowWidth);

        // Calculate & output performance stats
        output_stats(app, &tstart, frames);

        // void (*f) = output_stats; (void)f;

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
