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
#include "scene.h"
#include "types.h"
#include "vector.h"


static void init_app(App *app);
static void init_screen(App *app);
static void init_world(App *app);
static void run_render_loop(App *app);
static void output_stats(App *app, struct timespec *tstart, uint64_t frames);
static bool keyboard_esc_pressed();
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

static void init_app(App *app)
{
    setbuf(stdout, NULL);   // Disable stdout buffering.

    setlocale(LC_NUMERIC, "");  // Set numeric locale, to get printf("%'f") to separate thousands in numbers with commas ","

    // Seed the random number generator.
    struct timespec tnow;
    clock_gettime(CLOCK_MONOTONIC, &tnow);
    random_seed(tnow.tv_nsec);

    app->sdlWindow = NULL;
    app->sdlRenderer = NULL;
    app->sdlTexture = NULL;
}

static void init_screen(App *app)
{
    app->windowWidth    = WINDOW_WIDTH;
    app->windowHeight   = WINDOW_HEIGHT;

    renderer_init(app);

    // SDL_Init(SDL_INIT_VIDEO);
    // SDL_CreateWindowAndRenderer(app->windowWidth, app->windowHeight, 0, &app->window, &app->renderer);
    // // SDL_RenderSetScale(renderer, 4, 4);

    // SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
    // SDL_RenderClear(app->renderer);
    // SDL_RenderPresent(app->renderer);
}

static void init_world(App *app)
{
    Vector3 camOrigin;
    Vector3 camDirection;

    // Choose one of the available camera configurations.
    CameraConfig cc = CAMERA_CONFIG;
    switch (CAMERA_CONFIG) {
        case CC_z_0:
            // Camera is centered at [0, 0, 0] and looking towards the y axis.
            camOrigin       = (Vector3){.x = 0, .y = 0, .z = 0};
            camDirection    = (Vector3){.x = 0, .y = 1, .z = 0};
            break;

        case CC_z_15_downwards:
            // Camera is slightly above ground (z=15) and looking towards the y axis, at a slightly downward angle.
            camOrigin       = (Vector3){.x = 0, .y = 0, .z = 15};
            camDirection    = (Vector3){.x = 0, .y = 1, .z = -0.2};
            break;

        case CC_down__fov_40:
            // Camera is high up above ground and looking straight down onto the scene.
            camOrigin       = (Vector3){.x = 0, .y = 80, .z = 200};
            camDirection    = (Vector3){.x = 0, .y = 0.001, .z = -1};
            break;

        default:
            log_err("Fatal error: unknown camera configuration used: %d", cc);
            exit(1);
    }

    vector3_to_unit(&camDirection);     // Camera direction vector must be a unit (normalized to length 1) vector.

    Ray camCenterRay = {
        .origin     = camOrigin,
        .direction  = camDirection,
    };
    cam_set(&app->camera, &camCenterRay);

    init_scene(&app->scene);
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
        if (keyboard_esc_pressed()) {
            printf("User pressed the Esc key, exiting.\n");
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

static bool keyboard_esc_pressed()
{
    SDL_Event event;

    // Process all events currently in the event queue.
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
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
