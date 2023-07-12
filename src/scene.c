#include "main.h"
#include "rtalloc.h"
#include "scene.h"


static void scene_6_spheres__fov_90(Scene *scene);
static void scene_6_spheres__fov_40__cam_z_0(Scene *scene);
static void scene_6_spheres__fov_40__cam_z_15_downwards(Scene *scene);
static void scene_6_spheres__fov_40__cam_z_15_downwards_v2(Scene *scene);
static void scene_6_spheres__fov_40__cam_z_15_downwards_v3(Scene *scene);
static void scene_7_spheres__fov_40__cam_z_15_downwards(Scene *scene);
static void scene_camera_testing_1_sphere__fov_90(Scene *scene);
static void scene_camera_testing_4_spheres__fov_90(Scene *scene);
static void scene_camera_testing_4_spheres__fov_40(Scene *scene);
static void scene_rt_testing__1_sphere_center__fov_40(Scene *scene);
static void scene_rt_testing__1_sphere_inside__fov_40(Scene *scene);
static void sky_ambient_gray_07(Scene *scene);
static void sky_gradient_blue(Scene *scene);
static void sky_ambient_blue(Scene *scene);

static void add_light(Scene *scene, Sphere sphere, Color color);
static void add_metal_sphere(Scene *scene, Sphere sphere, double fuzziness);
static void add_sphere(Scene *scene, Sphere sphere);
static void add_sphere_ptr(Scene *scene, Sphere *sphere);


void init_scene(Scene *scene)
{
    scene->spheresLength = 0;

    // Choose one of the available scene configurations (descriptions inside each function).
    SceneConfig sc = SCENE_CONFIG;
    switch (sc) {
        case SC_none:
            break;

        case SC_6_spheres__fov_90:
            scene_6_spheres__fov_90(scene); break;

        case SC_6_spheres__fov_40__cam_z_0:
            scene_6_spheres__fov_40__cam_z_0(scene); break;

        case SC_6_spheres__fov_40__cam_z_15_downwards:
            scene_6_spheres__fov_40__cam_z_15_downwards(scene); break;

        case SC_6_spheres__fov_40__cam_z_15_downwards_v2:
            scene_6_spheres__fov_40__cam_z_15_downwards_v2(scene); break;

        case SC_6_spheres__fov_40__cam_z_15_downwards_v3:
            scene_6_spheres__fov_40__cam_z_15_downwards_v3(scene); break;

        case SC_7_spheres__fov_40__cam_z_15_downwards:
            scene_7_spheres__fov_40__cam_z_15_downwards(scene); break;

        case SC_camera_testing_1_sphere_fov_90:
            scene_camera_testing_1_sphere__fov_90(scene); break;

        case SC_camera_testing_4_spheres_fov_90:
            scene_camera_testing_4_spheres__fov_90(scene); break;

        case SC_camera_testing_4_spheres_fov_40:
            scene_camera_testing_4_spheres__fov_40(scene); break;

        case SC_rt_testing__1_sphere_center__fov_40:
            scene_rt_testing__1_sphere_center__fov_40(scene); break;

        case SC_rt_testing__1_sphere_inside__fov_40:
            scene_rt_testing__1_sphere_inside__fov_40(scene); break;

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
            sky_ambient_gray_07(scene); break;

        case SK_gradient_blue:
            sky_gradient_blue(scene); break;

        case SK_ambient_blue:
            sky_ambient_blue(scene); break;

        default:
            log_err("Fatal error: unknown sky configuration used: %d", sk);
            exit(1);
    }
}

static void scene_6_spheres__fov_90(Scene *scene)
{
    // Standard 6 sphere scene. FOV 90.

    // add_sphere(scene, (Sphere){.center = {.x = 0, .y = 30, .z = 0}, .radius = 10, .material = &matMatte, .color = COLOR_RED});              // Center sphere.
    add_sphere(scene, (Sphere){.center = {.x = 24, .y = 40, .z = 17}, .radius = 10, .material = &matMatte, .color = COLOR_GREEN});          // Top right sphere.
    add_sphere(scene, (Sphere){.center = {.x = -18, .y = 30, .z = -4}, .radius = 5, .material = &matMatte, .color = COLOR_BLUE});           // Center left sphere (small).

    add_sphere(scene, (Sphere){.center = {.x = 0, .y = 30, .z = 0}, .radius = 10, .material = &matMatte, .color = COLOR_RED});              // Center sphere (the big one).

    add_sphere(scene, (Sphere){.center = {.x = -8, .y = 12.5, .z = -8}, .radius = 3, .material = &matMatte, .color = COLOR_BLUE});          // Bottom left sphere (small).
    add_sphere(scene, (Sphere){.center = {.x = 0, .y = 12.5, .z = -8}, .radius = 3, .material = &matMatte, .color = COLOR_RED});            // Bottom center sphere (small).
    add_sphere(scene, (Sphere){.center = {.x = 8, .y = 12.5, .z = -8}, .radius = 3, .material = &matMatte, .color = COLOR_GREEN});          // Bottom right sphere (small).

    add_light(scene, (Sphere){.center = {.x = -9, .y = 20, .z = 10}, .radius = 3, .material = &matLight}, (Color)COLOR_LIGHT);              // A light.

    add_sphere(scene, (Sphere){.center = {.x = 0, .y = 220, .z = -2000}, .radius = 2000, .material = &matMatte, .color = COLOR_GROUND});    // Ground sphere.
}

static void scene_6_spheres__fov_40__cam_z_0(Scene *scene)
{
    // Standard 6 sphere scene. FOV 40. Camera origin and direction z = 0 (camera looking parallel to y axis).
    // At FOV 40 the bottom spheres only partially fit in the scene.

    add_sphere(scene, (Sphere){.center = {.x = 24, .y = 120, .z = 23}, .radius = 10, .material = &matMatte, .color = COLOR_GREEN});         // Top right sphere.
    add_sphere(scene, (Sphere){.center = {.x = 0, .y = 90, .z = 6}, .radius = 10, .material = &matMatte, .color = COLOR_RED});              // Center sphere (the big one).
    add_sphere(scene, (Sphere){.center = {.x = -18, .y = 90, .z = 1}, .radius = 5, .material = &matMatte, .color = COLOR_BLUE});            // Center left sphere (small).

    add_sphere(scene, (Sphere){.center = {.x = -8, .y = 32, .z = -6}, .radius = 3, .material = &matMatte, .color = COLOR_BLUE});            // Bottom left sphere (small).
    add_sphere(scene, (Sphere){.center = {.x = 0, .y = 32, .z = -6}, .radius = 3, .material = &matMatte, .color = COLOR_RED});              // Bottom center sphere (small).
    add_sphere(scene, (Sphere){.center = {.x = 8, .y = 32, .z = -6}, .radius = 3, .material = &matMatte, .color = COLOR_GREEN});            // Bottom right sphere (small).

    add_light(scene, (Sphere){.center = {.x = -9, .y = 80, .z = 16}, .radius = 3, .material = &matLight}, (Color)COLOR_LIGHT);              // A light.

    add_sphere(scene, (Sphere){.center = {.x = 0, .y = 220, .z = -2000}, .radius = 2000, .material = &matMatte, .color = COLOR_GROUND});    // Ground sphere.
}

static void scene_6_spheres__fov_40__cam_z_15_downwards(Scene *scene)
{
    // Standard 6 sphere scene. FOV 40. Camera origin z = 15 (slightly above ground) and looking slightly downwards.

    add_sphere(scene, (Sphere){.center = {.x = 24, .y = 120, .z = 20}, .radius = 10, .material = &matMatte, .color = COLOR_GREEN});         // Top right sphere.
    add_sphere(scene, (Sphere){.center = {.x = 0, .y = 90, .z = 6}, .radius = 10, .material = &matMatte, .color = COLOR_RED});              // Center sphere (the big one).
    add_sphere(scene, (Sphere){.center = {.x = -18, .y = 90, .z = 1}, .radius = 5, .material = &matMatte, .color = COLOR_BLUE});            // Center left sphere (small).

    add_sphere(scene, (Sphere){.center = {.x = -8, .y = 50, .z = -4}, .radius = 3, .material = &matMatte, .color = COLOR_BLUE});            // Bottom left sphere (small).
    add_sphere(scene, (Sphere){.center = {.x = 0, .y = 50, .z = -4}, .radius = 3, .material = &matMatte, .color = COLOR_RED});              // Bottom center sphere (small).
    add_sphere(scene, (Sphere){.center = {.x = 8, .y = 50, .z = -4}, .radius = 3, .material = &matMatte, .color = COLOR_GREEN});            // Bottom right sphere (small).

    add_light(scene, (Sphere){.center = {.x = -9, .y = 75, .z = 20}, .radius = 4, .material = &matLight}, (Color)COLOR_LIGHT);              // A light.

    add_sphere(scene, (Sphere){.center = {.x = 0, .y = 220, .z = -2000}, .radius = 2000, .material = &matMatte, .color = COLOR_GROUND});    // Ground sphere.
}

static void scene_6_spheres__fov_40__cam_z_15_downwards_v2(Scene *scene)
{
    // Standard 6 sphere scene. FOV 40. Camera origin z = 15 (slightly above ground) and looking slightly downwards. With metal spheres.

    add_sphere(scene, (Sphere){.center = {.x = 24, .y = 120, .z = 20}, .radius = 10, .material = &matMatte, .color = COLOR_HALF_GREEN});    // Top right sphere.
    add_metal_sphere(scene, (Sphere){.center = {.x = 0, .y = 90, .z = 6}, .radius = 10, .color = COLOR_QUARTER_RED}, 0.3);                  // Center red metal sphere (the big one).
    add_metal_sphere(scene, (Sphere){.center = {.x = -18, .y = 90, .z = 1}, .radius = 5, .color = COLOR_HALF_BLUE}, 0.0);                   // Center left blue metal sphere (small).

    add_sphere(scene, (Sphere){.center = {.x = -8, .y = 50, .z = -4}, .radius = 3, .material = &matMatte, .color = COLOR_BLUE});            // Bottom left sphere (small).
    add_sphere(scene, (Sphere){.center = {.x = 0, .y = 50, .z = -4}, .radius = 3, .material = &matMatte, .color = COLOR_RED});              // Bottom center sphere (small).
    add_sphere(scene, (Sphere){.center = {.x = 8, .y = 50, .z = -4}, .radius = 3, .material = &matMatte, .color = COLOR_GREEN});            // Bottom right sphere (small).

    add_light(scene, (Sphere){.center = {.x = -9, .y = 75, .z = 20}, .radius = 4, .material = &matLight}, (Color)COLOR_LIGHT);              // A light.

    add_sphere(scene, (Sphere){.center = {.x = 0, .y = 220, .z = -2000}, .radius = 2000, .material = &matMatte, .color = COLOR_GROUND});    // Ground sphere.
}

static void scene_6_spheres__fov_40__cam_z_15_downwards_v3(Scene *scene)
{
    // Standard 6 sphere scene. FOV 40. Camera origin z = 15 (slightly above ground) and looking slightly downwards. With metal+glass spheres.

    add_sphere(scene, (Sphere){.center = {.x = 24, .y = 120, .z = 20}, .radius = 10, .material = &matMatte, .color = COLOR_HALF_GREEN});    // Top right sphere.
    add_sphere_ptr(scene, sphere_glass_init(&(Sphere){.center = {.x = 0, .y = 90, .z = 6}, .radius = 10}));                                 // Center glass sphere (the big one).
    add_metal_sphere(scene, (Sphere){.center = {.x = -18, .y = 90, .z = 1}, .radius = 5, .color = COLOR_HALF_BLUE}, 0.0);                   // Center left sphere (small).

    add_sphere_ptr(scene, sphere_glass_init(&(Sphere){.center = {.x = -8, .y = 50, .z = -4}, .radius = 3}));                                // Bottom left glass sphere (small).
    add_sphere(scene, (Sphere){.center = {.x = 0, .y = 50, .z = -4}, .radius = 3, .material = &matMatte, .color = COLOR_RED});              // Bottom center sphere (small).
    add_sphere(scene, (Sphere){.center = {.x = 8, .y = 50, .z = -4}, .radius = 3, .material = &matMatte, .color = COLOR_GREEN});            // Bottom right sphere (small).

    add_light(scene, (Sphere){.center = {.x = -9, .y = 75, .z = 20}, .radius = 4, .material = &matLight}, (Color)COLOR_LIGHT);              // A light.

    add_sphere(scene, (Sphere){.center = {.x = 0, .y = 220, .z = -2000}, .radius = 2000, .material = &matMatte, .color = COLOR_GROUND});    // Ground sphere.
}

static void scene_7_spheres__fov_40__cam_z_15_downwards(Scene *scene)
{
    // Standard 7 sphere scene. FOV 40. Camera origin z = 15 (slightly above ground) and looking slightly downwards. With metal+glass spheres.

    add_metal_sphere(scene, (Sphere){.center = {.x = 24, .y = 120, .z = 20}, .radius = 10, .color = COLOR_HALF_GREEN}, 0.05);               // Top right sphere.
    add_sphere_ptr(scene, sphere_glass_init(&(Sphere){.center = {.x = 0, .y = 90, .z = 6}, .radius = 10}));                                 // Center glass sphere (the big one).
    add_metal_sphere(scene, (Sphere){.center = {.x = -18, .y = 90, .z = 1}, .radius = 5, .color = COLOR_HALF_BLUE}, 0.0);                   // Center left sphere (small).

    add_metal_sphere(scene, (Sphere){.center = {.x = 18, .y = 70, .z = 1}, .radius = 6, .color = COLOR_WHITE}, 0.0);                        // Middle right mirror sphere (big).

    add_sphere_ptr(scene, sphere_glass_init(&(Sphere){.center = {.x = -8, .y = 50, .z = -4}, .radius = 3}));                                // Bottom left glass sphere (small).
    add_sphere(scene, (Sphere){.center = {.x = 0, .y = 50, .z = -4}, .radius = 3, .material = &matMatte, .color = COLOR_RED});              // Bottom center sphere (small).
    add_sphere(scene, (Sphere){.center = {.x = 8, .y = 50, .z = -4}, .radius = 3, .material = &matMatte, .color = COLOR_GREEN});            // Bottom right sphere (small).

    add_light(scene, (Sphere){.center = {.x = -15, .y = 75, .z = 20}, .radius = 4, .material = &matLight}, (Color)COLOR_LIGHT);             // A light.

    add_sphere(scene, (Sphere){.center = {.x = 0, .y = 220, .z = -2000}, .radius = 2000, .material = &matMatte, .color = COLOR_GROUND});    // Ground sphere.
}

static void scene_camera_testing_1_sphere__fov_90(Scene *scene)
{
    // CAMERA TESTING SETUP: single big center sphere.
    // For rectangular image resolution only. For FOV 90.
    add_sphere(scene, (Sphere){.center = {.x = 0, .y = 1.4142135623730950488016887242097, .z = 0}, .radius = 1, .material = &matMatte, .color = COLOR_RED});
}

static void scene_camera_testing_4_spheres__fov_90(Scene *scene)
{
    // CAMERA TESTING SETUP: 4 spheres at top/right/bottom/left, diameter of each is 1/4 of screen width/height (i.e. they do not touch
    // each other).
    // For rectangular image resolution only. For FOV 90.
    add_sphere(scene, (Sphere){.center = {.x = 0, .y = 3.5355339059327376220042218105242, .z = 2.1213203435596425732025330863145}, .radius = 1, .material = &matMatte, .color = COLOR_RED});  // top
    add_sphere(scene, (Sphere){.center = {.x = -2.1213203435596425732025330863145, .y = 3.5355339059327376220042218105242, .z = 0}, .radius = 1, .material = &matMatte, .color = COLOR_RED}); // left
    add_sphere(scene, (Sphere){.center = {.x = 2.1213203435596425732025330863145, .y = 3.5355339059327376220042218105242, .z = 0}, .radius = 1, .material = &matMatte, .color = COLOR_RED});  // right
    add_sphere(scene, (Sphere){.center = {.x = 0, .y = 3.5355339059327376220042218105242, .z = -2.1213203435596425732025330863145}, .radius = 1, .material = &matMatte, .color = COLOR_RED}); // bottom
}

static void scene_camera_testing_4_spheres__fov_40(Scene *scene)
{
    // CAMERA TESTING SETUP: 4 spheres at top/right/bottom/left, diameter of each is 1/4 of screen width/height (i.e. they do not touch
    // each other).
    // For rectangular image resolution only. For FOV 40.
    add_sphere(scene, (Sphere){.center = {.x = 0, .y = 10.669157170675342809798718809418, .z = 2.8190778623577251521623278319742}, .radius = 1, .material = &matMatte, .color = COLOR_RED});  // top
    add_sphere(scene, (Sphere){.center = {.x = -2.8190778623577251521623278319742, .y = 10.669157170675342809798718809418, .z = 0}, .radius = 1, .material = &matMatte, .color = COLOR_RED}); // left
    add_sphere(scene, (Sphere){.center = {.x = 2.8190778623577251521623278319742, .y = 10.669157170675342809798718809418, .z = 0}, .radius = 1, .material = &matMatte, .color = COLOR_RED});  // right
    add_sphere(scene, (Sphere){.center = {.x = 0, .y = 10.669157170675342809798718809418, .z = -2.8190778623577251521623278319742}, .radius = 1, .material = &matMatte, .color = COLOR_RED}); // bottom
}

static void scene_rt_testing__1_sphere_center__fov_40(Scene *scene)
{
    // add_sphere(scene, (Sphere){.center = {.x = 0, .y = 15, .z = 0}, .radius = 2, .material = &matTest, .color = COLOR_WHITE});
    add_sphere_ptr(scene, sphere_glass_init(&(Sphere){.center = {.x = 0, .y = 15, .z = 0}, .radius = 2}));
}

static void scene_rt_testing__1_sphere_inside__fov_40(Scene *scene)
{
    // add_sphere(scene, (Sphere){.center = {.x = 0, .y = 1, .z = 0}, .radius = 2, .material = &matTest, .color = COLOR_WHITE});
    add_sphere_ptr(scene, sphere_glass_init(&(Sphere){.center = {.x = 0, .y = 1, .z = 0}, .radius = 2}));
}

static void sky_ambient_gray_07(Scene *scene)
{
    // Sky sphere, providing an ambient light (COLOR_BLACK is the Color equivalent of NULL).
    add_light(scene, (Sphere){.center = {.x = 0, .y = 0, .z = 0}, .radius = 20000, .material = &matLight}, (Color)COLOR_AMBIENT_LIGHT);
}

static void sky_gradient_blue(Scene *scene)
{
    // Sky sphere, providing a gradient (blue-white) light.
    add_sphere(scene, (Sphere){.center = {.x = 0, .y = 0, .z = 0}, .radius = 20000, .material = &matGradientSky, .color = COLOR_BLACK});
}

static void sky_ambient_blue(Scene *scene)
{
    // Sky sphere, providing a ambient blue-ish light.
    add_light(scene, (Sphere){.center = {.x = 0, .y = 0, .z = 0}, .radius = 20000, .material = &matLight}, (Color)COLOR_SKY);
}

static void add_light(Scene *scene, Sphere sphere, Color color)
{
    sphere.material = &matLight;

    MaterialDataLight *matData = rtalloc(sizeof(MaterialDataLight));
    matData->color = color;
    sphere.matData = matData;

    add_sphere(scene, sphere);
}

static void add_metal_sphere(Scene *scene, Sphere sphere, double fuzziness)
{
    sphere.material = &matMetal;

    MaterialDataMetal *matData = rtalloc(sizeof(MaterialDataMetal));
    matData->fuzziness = fuzziness;
    sphere.matData = matData;

    add_sphere(scene, sphere);
}

static void add_sphere(Scene *scene, Sphere sphere)
{
    add_sphere_ptr(scene, &sphere);
}

static void add_sphere_ptr(Scene *scene, Sphere *sphere)
{
    if (scene->spheresLength == SCENE_SPHERES_MAX) {
        log_err("Cannot add another sphere to the scene - scene already has maximum spheres");
        exit(1);
    }

    scene->spheres[scene->spheresLength] = *sphere;
    scene->spheresLength++;
}
