#include <stdio.h>

#include "material.h"

#include "color.h"
#include "ray.h"
#include "rtmath.h"
#include "vector.h"


static Color32 matte_hit(Scene *scene, Ray *ray, RTContext *rtContext, Sphere *sphere, Vector3 *pos);
static Color32 shaded_hit(Scene *scene, Ray *ray, RTContext *rtContext, Sphere *sphere, Vector3 *pos);
static Color32 sky_hit(Scene *scene, Ray *ray, RTContext *rtContext, Sphere *sphere, Vector3 *pos);
static Color32 ground_hit(Scene *scene, Ray *ray, RTContext *rtContext, Sphere *sphere, Vector3 *pos);
static Color32 light_hit(Scene *scene, Ray *ray, RTContext *rtContext, Sphere *sphere, Vector3 *pos);


Material matMatte = {
    // .type = MT_Shaded,
    // .scatter = matte_scatter,
    .hit = matte_hit,
};
Material matShaded = {
    // .type = MT_Shaded,
    // .scatter = matte_scatter,
    .hit = shaded_hit,
};
Material matGradientSky = {
    // .type = MT_Sky,
    .hit = sky_hit,
};
Material matGround = {
    // .type = MT_Ground,
    .hit = ground_hit,
};
Material matLight = {
    // .type = MT_Ground,
    .hit = light_hit,
};


// static uint32_t hits = 0;
static Color32 matte_hit(Scene *scene, Ray *ray, RTContext *rtContext, Sphere *sphere, Vector3 *pos)
{
    // (void)(scene);      // Disable gcc -Wextra "unused parameter" errors.
    (void)(ray);
    // (void)(sphere);
    // (void)(pos);

    // if (++hits > 100) {
    //     exit(1);
    // }

    Vector3 normal;
    calc_sphere_surface_normal(sphere, pos, &normal);

    Vector3 unitSphereRandomPoint;
    random_point_in_unit_sphere(&unitSphereRandomPoint);
    vector3_to_unit(&unitSphereRandomPoint);                // Must convert to a unit vector, otherwise addition math will be wrong.

    Vector3 bouncedRayDirection;
    vector3_add_to(&normal, &unitSphereRandomPoint, &bouncedRayDirection);
    vector3_to_unit(&bouncedRayDirection);

    Ray bouncedRay = (Ray){
        .origin = *pos,
        .direction = bouncedRayDirection,
    };

    // printf("\nmatte_hit()\n");
    // printf("rtContext->bounces = %d\n", rtContext->bounces);
    // printf("sphere = %p\n", sphere);
    // printf("ray origin .x = %f, .y = %f, .z = %f\n", ray->origin.x, ray->origin.y, ray->origin.z);
    // printf("ray direction .x = %f, .y = %f, .z = %f\n", ray->direction.x, ray->direction.y, ray->direction.z);

    // printf("pos .x = %f, .y = %f, .z = %f\n", pos->x, pos->y, pos->z);

    // printf("bouncedRay origin .x = %f, .y = %f, .z = %f\n", bouncedRay.origin.x, bouncedRay.origin.y, bouncedRay.origin.z);
    // printf("bouncedRay direction .x = %f, .y = %f, .z = %f\n", bouncedRay.direction.x, bouncedRay.direction.y, bouncedRay.direction.z);
    // printf("\n");

    Color32 bouncedRayIncomingColor;
    bool traceSuccess = ray_trace(rtContext, scene, &bouncedRay, &bouncedRayIncomingColor);

    // printf("traceSuccess = %d\n", traceSuccess);

    if (! traceSuccess) {
        // Could not find any incoming color (light), so just shade this pixel of the sphere with black.
        return (Color32)COLOR_BLACK;
    }

    // // For calculating the dot product between incoming `ray` and the `bouncedRay` - we need to reverse `bouncedRay` direction.
    // Vector3 bouncedRayReverseDirection = (Vector3){
    //     .x = -bouncedRay.direction.x, .y = -bouncedRay.direction.y, .z = -bouncedRay.direction.z,
    // };
    // double raysDotProduct = vector3_dot(&ray->direction, &bouncedRayReverseDirection);

    // double raysDotProduct = fabs(vector3_dot(&ray->direction, &bouncedRay.direction));
    // double bouncedRayIncomingLight =
    //         raysDotProduct
    //       * ((double)(bouncedRayIncomingColor.red + bouncedRayIncomingColor.green + bouncedRayIncomingColor.blue) / (256*3));

    // double bouncedRayIncomingLight =
    //         ((double)(bouncedRayIncomingColor.red + bouncedRayIncomingColor.green + bouncedRayIncomingColor.blue) / (256*3));

    // printf("bouncedRayIncomingColor .red = %d, .green = %d, .blue = %d\n", bouncedRayIncomingColor.red, bouncedRayIncomingColor.green, bouncedRayIncomingColor.blue);
    // printf("raysDotProduct = %f\n", raysDotProduct);
    // printf("bouncedRayIncomingLight = %f\n", bouncedRayIncomingLight);

    Color *sphereColor = &sphere->color;

    // We're using 64 bits to store the resulting colors after multiplication, to avoid overflow.
    // Though not sure if that is really needed, because we're multiplying sphere colors that should be 8 bit (even if they're stored as
    // 32 bit) with a 32 bit bounce ray color (which is first divided by 256 (8 bits)). So _theoretically_ i think the multiplied value
    // cannot exceed 32 bits. But we're using 64 bits here anyway, just in case.
    uint64_t red   = (uint64_t)round((double)sphereColor->red * ((double)bouncedRayIncomingColor.red / 256)),
             green = (uint64_t)round((double)sphereColor->green * ((double)bouncedRayIncomingColor.green / 256)),
             blue  = (uint64_t)round((double)sphereColor->blue * ((double)bouncedRayIncomingColor.blue / 256));

    return (Color32){
        // // Note that we must cap the colors of the matte surface within the expected 8 bits
        // // (even though we are operating on Color32, where each component is 32 bits).
        // .red    = min((uint32_t)255, (uint32_t)round((double)sphereColor->red * ((double)bouncedRayIncomingColor.red / 256))),
        // .green  = min((uint32_t)255, (uint32_t)round((double)sphereColor->green * ((double)bouncedRayIncomingColor.green / 256))),
        // .blue   = min((uint32_t)255, (uint32_t)round((double)sphereColor->blue * ((double)bouncedRayIncomingColor.blue / 256))),

        // Must cap the resulting colors at 32 bits.
        .red    = min(0xFFFFFFFE, red),
        .green  = min(0xFFFFFFFE, green),
        .blue   = min(0xFFFFFFFE, blue),
    };
}

static Color32 shaded_hit(Scene *scene, Ray *ray, RTContext *rtContext, Sphere *sphere, Vector3 *pos)
{
    (void)(scene);      // Disable gcc -Wextra "unused parameter" errors.
    (void)(ray);
    (void)(rtContext);
    (void)(sphere);
    (void)(pos);

    // printf("\nshaded_hit()\n");

    // // @TODO: Implement me!
    // return (Color)COLOR_BLACK;

    // Ray scatteredRay;
    // minSphere->material->scatter(ray, &scatteredRay);

    // Calculate the sphere surface normal vector at `pos`.
    // I.e. a normalized (unit) vector from `sphere->center` to `pos`.
    Vector3 normal;
    calc_sphere_surface_normal(sphere, pos, &normal);

    return (Color32){
        .red = floor((normal.x + 1) * 128),
        .green = floor((normal.y + 1) * 128),
        .blue = floor((normal.z + 1) * 128),
    };
}

static Color skyTopColor = (Color)COLOR_GRADIENT_SKY_TOP;
static Color skyBottomColor = (Color)COLOR_GRADIENT_SKY_BOTTOM;
static Color32 skyBottomColor32 = (Color32)COLOR_GRADIENT_SKY_BOTTOM;

static Color32 sky_hit(Scene *scene, Ray *ray, RTContext *rtContext, Sphere *sphere, Vector3 *pos)
{
    (void)(scene);      // Disable gcc -Wextra "unused parameter" errors.
    (void)(rtContext);
    (void)(sphere);
    (void)(pos);

    // printf("\nsky_hit()\n");

    double y = ray->direction.y;
    if (y < 0) {
        return skyBottomColor32;
    } else {
        return gradient(&skyBottomColor, &skyTopColor, 0, 1, ray->direction.y);
    }
}

static Color32 ground_hit(Scene *scene, Ray *ray, RTContext *rtContext, Sphere *sphere, Vector3 *pos)
{
    (void)(scene);      // Disable gcc -Wextra "unused parameter" errors.
    (void)(ray);
    (void)(rtContext);
    (void)(sphere);
    (void)(pos);

    // printf("\nground_hit()\n");

    return (Color32)COLOR_GROUND;
}

static Color32 light_hit(Scene *scene, Ray *ray, RTContext *rtContext, Sphere *sphere, Vector3 *pos)
{
    (void)(scene);      // Disable gcc -Wextra "unused parameter" errors.
    (void)(ray);
    (void)(rtContext);
    (void)(pos);

    // printf("\nlight_hit()\n");

    MaterialLightData *matData = sphere->matData;

    // printf("matData = %p\n", matData);
    // printf("matData->color .red = %d, .green = %d, .blue = %d", matData->color.red, matData->color.green, matData->color.blue);
    // exit(1);

    return matData->color;
}

