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
static void add_metal_sphere(App *app, Sphere sphere, double fuzziness);
static void add_sphere(App *app, Sphere sphere);
static void add_sphere_ptr(App *app, Sphere *sphere);
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

    init_scene(app);
}

static void init_scene(App *app)
{
    app->scene.spheresLength = 0;

    // Choose one of the available scene configurations (descriptions inside each function).
    SceneConfig sc = SCENE_CONFIG;
    switch (sc) {
        case SC_none:
            break;

        case SC_6_spheres__fov_90:
            scene_6_spheres__fov_90(app); break;

        case SC_6_spheres__fov_40__cam_z_0:
            scene_6_spheres__fov_40__cam_z_0(app); break;

        case SC_6_spheres__fov_40__cam_z_15_downwards:
            scene_6_spheres__fov_40__cam_z_15_downwards(app); break;

        case SC_6_spheres__fov_40__cam_z_15_downwards_v2:
            scene_6_spheres__fov_40__cam_z_15_downwards_v2(app); break;

        case SC_6_spheres__fov_40__cam_z_15_downwards_v3:
            scene_6_spheres__fov_40__cam_z_15_downwards_v3(app); break;

        case SC_camera_testing_1_sphere_fov_90:
            scene_camera_testing_1_sphere__fov_90(app); break;

        case SC_camera_testing_4_spheres_fov_90:
            scene_camera_testing_4_spheres__fov_90(app); break;

        case SC_camera_testing_4_spheres_fov_40:
            scene_camera_testing_4_spheres__fov_40(app); break;

        default:
            log_err("Fatal error: unknown scene configuration used: %d", sc);
            exit(1);
    }

    // Choose one of the available sky configurations (descriptions inside each function).
    SkyConfig sk = SKY_CONFIG;
    switch (sk) {
        case SK_none:
            break;

        case SK_ambient_gray_07:
            sky_ambient_gray_07(app); break;

        case SK_gradient_blue:
            sky_gradient_blue(app); break;

        case SK_ambient_blue:
            sky_ambient_blue(app); break;

        default:
            log_err("Fatal error: unknown sky configuration used: %d", sk);
            exit(1);
    }
}

void scene_6_spheres__fov_90(App *app)
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

void scene_6_spheres__fov_40__cam_z_0(App *app)
{
    // Standard 6 sphere scene. FOV 40. Camera origin and direction z = 0 (camera looking parallel to y axis).
    // At FOV 40 the bottom spheres only partially fit in the scene.

    add_sphere(app, (Sphere){.center = {.x = 24, .y = 120, .z = 23}, .radius = 10, .material = &matMatte, .color = COLOR_GREEN});   // Top right sphere.
    add_sphere(app, (Sphere){.center = {.x = 0, .y = 90, .z = 6}, .radius = 10, .material = &matMatte, .color = COLOR_RED});        // Center sphere (the big one).
    add_sphere(app, (Sphere){.center = {.x = -18, .y = 90, .z = 1}, .radius = 5, .material = &matMatte, .color = COLOR_BLUE});      // Center left sphere (small).

    add_sphere(app, (Sphere){.center = {.x = -8, .y = 32, .z = -6}, .radius = 3, .material = &matMatte, .color = COLOR_BLUE});   // Bottom left sphere (small).
    add_sphere(app, (Sphere){.center = {.x = 0, .y = 32, .z = -6}, .radius = 3, .material = &matMatte, .color = COLOR_RED});     // Bottom center sphere (small).
    add_sphere(app, (Sphere){.center = {.x = 8, .y = 32, .z = -6}, .radius = 3, .material = &matMatte, .color = COLOR_GREEN});   // Bottom right sphere (small).

    add_light(app, (Sphere){.center = {.x = -9, .y = 80, .z = 16}, .radius = 3, .material = &matLight}, (Color)COLOR_LIGHT);    // A light.

    add_sphere(app, (Sphere){.center = {.x = 0, .y = 220, .z = -2000}, .radius = 2000, .material = &matMatte, .color = COLOR_GROUND});  // Ground sphere.
}

void scene_6_spheres__fov_40__cam_z_15_downwards(App *app)
{
    // Standard 6 sphere scene. FOV 40. Camera origin z = 15 (slightly above ground) and looking slightly downwards.

    add_sphere(app, (Sphere){.center = {.x = 24, .y = 120, .z = 20}, .radius = 10, .material = &matMatte, .color = COLOR_GREEN});   // Top right sphere.
    add_sphere(app, (Sphere){.center = {.x = 0, .y = 90, .z = 6}, .radius = 10, .material = &matMatte, .color = COLOR_RED});        // Center sphere (the big one).
    add_sphere(app, (Sphere){.center = {.x = -18, .y = 90, .z = 1}, .radius = 5, .material = &matMatte, .color = COLOR_BLUE});      // Center left sphere (small).

    add_sphere(app, (Sphere){.center = {.x = -8, .y = 50, .z = -4}, .radius = 3, .material = &matMatte, .color = COLOR_BLUE});   // Bottom left sphere (small).
    add_sphere(app, (Sphere){.center = {.x = 0, .y = 50, .z = -4}, .radius = 3, .material = &matMatte, .color = COLOR_RED});     // Bottom center sphere (small).
    add_sphere(app, (Sphere){.center = {.x = 8, .y = 50, .z = -4}, .radius = 3, .material = &matMatte, .color = COLOR_GREEN});   // Bottom right sphere (small).

    add_light(app, (Sphere){.center = {.x = -9, .y = 75, .z = 20}, .radius = 4, .material = &matLight}, (Color)COLOR_LIGHT);    // A light.

    add_sphere(app, (Sphere){.center = {.x = 0, .y = 220, .z = -2000}, .radius = 2000, .material = &matMatte, .color = COLOR_GROUND});  // Ground sphere.
}

void scene_6_spheres__fov_40__cam_z_15_downwards_v2(App *app)
{
    // Standard 6 sphere scene. FOV 40. Camera origin z = 15 (slightly above ground) and looking slightly downwards. With metal spheres.

    add_sphere(app, (Sphere){.center = {.x = 24, .y = 120, .z = 20}, .radius = 10, .material = &matMatte, .color = COLOR_HALF_GREEN});          // Top right sphere.
    add_metal_sphere(app, (Sphere){.center = {.x = 0, .y = 90, .z = 6}, .radius = 10, .material = &matMetal, .color = COLOR_QUARTER_RED}, 0.3); // Center sphere (the big one).
    add_metal_sphere(app, (Sphere){.center = {.x = -18, .y = 90, .z = 1}, .radius = 5, .material = &matMatte, .color = COLOR_HALF_BLUE}, 0.0);  // Center left sphere (small).

    add_sphere(app, (Sphere){.center = {.x = -8, .y = 50, .z = -4}, .radius = 3, .material = &matMatte, .color = COLOR_BLUE});  // Bottom left sphere (small).
    add_sphere(app, (Sphere){.center = {.x = 0, .y = 50, .z = -4}, .radius = 3, .material = &matMatte, .color = COLOR_RED});    // Bottom center sphere (small).
    add_sphere(app, (Sphere){.center = {.x = 8, .y = 50, .z = -4}, .radius = 3, .material = &matMatte, .color = COLOR_GREEN});  // Bottom right sphere (small).

    add_light(app, (Sphere){.center = {.x = -9, .y = 75, .z = 20}, .radius = 4, .material = &matLight}, (Color)COLOR_LIGHT);    // A light.

    add_sphere(app, (Sphere){.center = {.x = 0, .y = 220, .z = -2000}, .radius = 2000, .material = &matMatte, .color = COLOR_GROUND});  // Ground sphere.
}

void scene_6_spheres__fov_40__cam_z_15_downwards_v3(App *app)
{
    // Standard 6 sphere scene. FOV 40. Camera origin z = 15 (slightly above ground) and looking slightly downwards. With metal+glass spheres.

    add_sphere(app, (Sphere){.center = {.x = 24, .y = 120, .z = 20}, .radius = 10, .material = &matMatte, .color = COLOR_HALF_GREEN});          // Top right sphere.
    // add_metal_sphere(app, (Sphere){.center = {.x = 0, .y = 90, .z = 6}, .radius = 10, .material = &matMetal, .color = COLOR_QUARTER_RED}, 0.3); // Center sphere (the big one).
    add_sphere_ptr(app, sphere_glass_init(&(Sphere){.center = {.x = 0, .y = 90, .z = 6}, .radius = 10}));                                     // Center sphere (the big one).
    add_metal_sphere(app, (Sphere){.center = {.x = -18, .y = 90, .z = 1}, .radius = 5, .material = &matMatte, .color = COLOR_HALF_BLUE}, 0.0);  // Center left sphere (small).

    add_sphere_ptr(app, sphere_glass_init(&(Sphere){.center = {.x = -8, .y = 50, .z = -4}, .radius = 3}));                     // Bottom left sphere (small).
    add_sphere(app, (Sphere){.center = {.x = 0, .y = 50, .z = -4}, .radius = 3, .material = &matMatte, .color = COLOR_RED});    // Bottom center sphere (small).
    add_sphere(app, (Sphere){.center = {.x = 8, .y = 50, .z = -4}, .radius = 3, .material = &matMatte, .color = COLOR_GREEN});  // Bottom right sphere (small).

    add_light(app, (Sphere){.center = {.x = -9, .y = 75, .z = 20}, .radius = 4, .material = &matLight}, (Color)COLOR_LIGHT);    // A light.

    add_sphere(app, (Sphere){.center = {.x = 0, .y = 220, .z = -2000}, .radius = 2000, .material = &matMatte, .color = COLOR_GROUND});  // Ground sphere.
}

void scene_camera_testing_1_sphere__fov_90(App *app)
{
    // CAMERA TESTING SETUP: single big center sphere.
    // For rectangular image resolution only. For FOV 90.
    add_sphere(app, (Sphere){.center = {.x = 0, .y = 1.4142135623730950488016887242097, .z = 0}, .radius = 1, .material = &matMatte, .color = COLOR_RED});
}

void scene_camera_testing_4_spheres__fov_90(App *app)
{
    // CAMERA TESTING SETUP: 4 spheres at top/right/bottom/left, diameter of each is 1/4 of screen width/height (i.e. they do not touch
    // each other).
    // For rectangular image resolution only. For FOV 90.
    add_sphere(app, (Sphere){.center = {.x = 0, .y = 3.5355339059327376220042218105242, .z = 2.1213203435596425732025330863145}, .radius = 1, .material = &matMatte, .color = COLOR_RED});  // top
    add_sphere(app, (Sphere){.center = {.x = -2.1213203435596425732025330863145, .y = 3.5355339059327376220042218105242, .z = 0}, .radius = 1, .material = &matMatte, .color = COLOR_RED}); // left
    add_sphere(app, (Sphere){.center = {.x = 2.1213203435596425732025330863145, .y = 3.5355339059327376220042218105242, .z = 0}, .radius = 1, .material = &matMatte, .color = COLOR_RED});  // right
    add_sphere(app, (Sphere){.center = {.x = 0, .y = 3.5355339059327376220042218105242, .z = -2.1213203435596425732025330863145}, .radius = 1, .material = &matMatte, .color = COLOR_RED}); // bottom
}

void scene_camera_testing_4_spheres__fov_40(App *app)
{
    // CAMERA TESTING SETUP: 4 spheres at top/right/bottom/left, diameter of each is 1/4 of screen width/height (i.e. they do not touch
    // each other).
    // For rectangular image resolution only. For FOV 40.
    add_sphere(app, (Sphere){.center = {.x = 0, .y = 10.669157170675342809798718809418, .z = 2.8190778623577251521623278319742}, .radius = 1, .material = &matMatte, .color = COLOR_RED});  // top
    add_sphere(app, (Sphere){.center = {.x = -2.8190778623577251521623278319742, .y = 10.669157170675342809798718809418, .z = 0}, .radius = 1, .material = &matMatte, .color = COLOR_RED}); // left
    add_sphere(app, (Sphere){.center = {.x = 2.8190778623577251521623278319742, .y = 10.669157170675342809798718809418, .z = 0}, .radius = 1, .material = &matMatte, .color = COLOR_RED});  // right
    add_sphere(app, (Sphere){.center = {.x = 0, .y = 10.669157170675342809798718809418, .z = -2.8190778623577251521623278319742}, .radius = 1, .material = &matMatte, .color = COLOR_RED}); // bottom
}

void sky_ambient_gray_07(App *app)
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
    sphere.material = &matLight;

    MaterialDataLight *matData = rtalloc(sizeof(MaterialDataLight));
    matData->color = color;
    sphere.matData = matData;

    add_sphere(app, sphere);
}

static void add_metal_sphere(App *app, Sphere sphere, double fuzziness)
{
    sphere.material = &matMetal;

    MaterialDataMetal *matData = rtalloc(sizeof(MaterialDataMetal));
    matData->fuzziness = fuzziness;
    sphere.matData = matData;

    add_sphere(app, sphere);
}

static void add_sphere(App *app, Sphere sphere)
{
    add_sphere_ptr(app, &sphere);
}

static void add_sphere_ptr(App *app, Sphere *sphere)
{
    if (app->scene.spheresLength == SCENE_SPHERES_MAX) {
        log_err("Cannot add another sphere to the scene - scene already has maximum spheres");
        exit(1);
    }

    app->scene.spheres[app->scene.spheresLength] = *sphere;
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
