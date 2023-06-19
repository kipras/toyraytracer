#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "main.h"

#include "ray.h"
#include "vector.h"


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

    app = create_app();
    init_screen(app);
    init_world(app);
    init_precalc(app);
    render(app);

    return 0;
}

void log_err(char *err)
{
    fprintf(stderr, "%s", err);
}

App * create_app()
{
    App *app;

    setbuf(stdout, NULL);

    app = ralloc(sizeof(App));
    if (app == NULL) {
        log_err("Fatal error: could not allocate App. Out of memory?");
        exit(1);
    }

    app->window = NULL;
    app->renderer = NULL;

    return app;
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
    // app->camera.from.x = 0;
    // app->camera.from.y = 0;
    // app->camera.from.z = 0;

    // // Camera is looking parallel to the ground (i.e. z=0 (from the range [-1; 1])), in the direction of y=1, x=0 (i.e. "up")
    // app->camera.direction.hor  = 0;     // Look in the direction of y=1, x=0 (i.e. "up" on the horizontal plane)
    // app->camera.direction.vert = 0;     // Look "sideways" on the vertical plane

    app->fov = FOV;

    app->scene.spheres[0] = (Sphere){.center = {.x = 0, .y = 0, .z = -10}, .radius = 5, .materialType = Matte, .color = COLOR_RED};
    app->scene.spheresLength = 1;
}

// void init_precalc(App *app)
void init_precalc()
{
}

void render(App *app)
{
    render_frame(app);

//    // Render 100 rays per pixel
//    for (uint64_t i = 0; i < 100; i++) {
//        render_frame(app);
//    }

    // SDL_SetRenderDrawColor(app->renderer, 255, 255, 255, 255);
    // SDL_RenderDrawPoint(app->renderer, WINDOW_WIDTH/2, WINDOW_HEIGHT/2);
    // SDL_RenderPresent(app->renderer);

    SDL_Delay(3000);
}

void render_frame(App *app)
{
    Vector3 cameraPosition = {.x = 0, .y = 0, .z = 0};
    Ray ray = {
        .origin = cameraPosition,
        .direction = {.x = 0, .y = 0, .z = -1},
    };

    uint32_t windowMidHeight = app->windowHeight >> 1;
    uint32_t windowMidWidth = app->windowWidth >> 1;

    for (uint32_t screenY = 0; screenY < app->windowHeight; screenY++) {
        // Ray direction is from [y=1, x=-1] (top left corner) to [y=-1, x=1] (bottom right corner).
        // Depth (z) remains unchanged at -1.
        ray.direction.y = ((double)screenY / windowMidHeight) - 1;
        // printf("\n");
        // printf("ray.direction.y = %f\n", ray.direction.y);
        for (uint32_t screenX = 0; screenX < app->windowWidth; screenX++) {
            ray.direction.x = ((double)screenX / windowMidWidth) - 1;
            // printf("ray.direction.x = %f\n", ray.direction.x);

            Sphere *sphereList = app->scene.spheres;
            for (uint32_t i = 0; i < app->scene.spheresLength; i++) {
                Sphere *sphere = &sphereList[i];
                if (ray_hit_sphere(&ray, sphere)) {
                    Color *color = &sphere->color;
                    SDL_SetRenderDrawColor(app->renderer, color->red, color->green, color->blue, 255);
                    SDL_RenderDrawPoint(app->renderer, screenX, screenY);
                }
            }

            // Color color = {
            //     .red = (ray.direction.x + 1) * 128,
            //     .green = (ray.direction.y + 1) * 128,
            //     .blue = 0,
            // };
            // SDL_SetRenderDrawColor(app->renderer, color.red, color.green, color.blue, 255);
            // SDL_RenderDrawPoint(app->renderer, screenX, screenY);
        }
    }

    // SDL_SetRenderDrawColor(app->renderer, color->red, color->green, color->blue, 255);
    // SDL_RenderDrawPoint(app->renderer, x, y);

    SDL_RenderPresent(app->renderer);
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
