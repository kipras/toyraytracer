#include <float.h>
#include <stdio.h>

#include "material.h"

#include "color.h"
#include "ray.h"
#include "rtmath.h"
#include "vector.h"


static Color matte_hit(Scene *scene, Ray *ray, RTContext *rtContext, Sphere *sphere, Vector3 *pos);
static Color shaded_hit(Scene *scene, Ray *ray, RTContext *rtContext, Sphere *sphere, Vector3 *pos);
static Color sky_hit(Scene *scene, Ray *ray, RTContext *rtContext, Sphere *sphere, Vector3 *pos);
static Color ground_hit(Scene *scene, Ray *ray, RTContext *rtContext, Sphere *sphere, Vector3 *pos);
static Color light_hit(Scene *scene, Ray *ray, RTContext *rtContext, Sphere *sphere, Vector3 *pos);
static Color metal_hit(Scene *scene, Ray *ray, RTContext *rtContext, Sphere *sphere, Vector3 *pos);
static inline void mirror_reflect(Vector3 *incoming, Vector3 *normal, double fuzziness, Vector3 *reflected);


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
Material matMetal = {
    .hit = metal_hit,
};


// static uint32_t hits = 0;
static Color matte_hit(Scene *scene, Ray *ray, RTContext *rtContext, Sphere *sphere, Vector3 *pos)
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

    // Generate a random point/vector for the scattered ray that will scatter from the `pos` (where the incoming ray
    // hits this surface). How we will generate the scattered ray depends on MATTE_DIFFUSE_TYPE.
    MatteDiffuseAlgo mda = MATTE_DIFFUSE_ALGO;
    Vector3 bouncedRayDirection;
    if (mda == MDA_randomVectorInHemisphere) {
        random_point_in_hemisphere(&bouncedRayDirection, &normal);
        vector3_to_unit(&bouncedRayDirection);
    } else {
        // MDA_randomVectorInUnitSphere or MDA_randomUnitVectorInUnitSphere
        Vector3 unitSphereRandomPoint;
        random_point_in_unit_sphere(&unitSphereRandomPoint);
        if (mda == MDA_randomUnitVectorInUnitSphere) {
            // Must convert to a unit vector, otherwise addition math will be wrong.
            vector3_to_unit(&unitSphereRandomPoint);
        }
        vector3_add_to(&normal, &unitSphereRandomPoint, &bouncedRayDirection);
        vector3_to_unit(&bouncedRayDirection);
    }

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

    Color bouncedRayIncomingColor;
    bool traceSuccess = ray_trace(rtContext, scene, &bouncedRay, &bouncedRayIncomingColor);

    // printf("traceSuccess = %d\n", traceSuccess);

    if (! traceSuccess) {
        // Could not find any incoming color (light), so just shade this pixel of the sphere with black.
        return (Color)COLOR_BLACK;
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

    return (Color){
        .red    = sphereColor->red   * bouncedRayIncomingColor.red,
        .green  = sphereColor->green * bouncedRayIncomingColor.green,
        .blue   = sphereColor->blue  * bouncedRayIncomingColor.blue,
    };
}

static Color shaded_hit(Scene *scene, Ray *ray, RTContext *rtContext, Sphere *sphere, Vector3 *pos)
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

    return (Color){
        .red = floor((normal.x + 1) * 128),
        .green = floor((normal.y + 1) * 128),
        .blue = floor((normal.z + 1) * 128),
    };
}

static Color skyTopColor = (Color)COLOR_GRADIENT_SKY_TOP;
static Color skyBottomColor = (Color)COLOR_GRADIENT_SKY_BOTTOM;

static Color sky_hit(Scene *scene, Ray *ray, RTContext *rtContext, Sphere *sphere, Vector3 *pos)
{
    (void)(scene);      // Disable gcc -Wextra "unused parameter" errors.
    (void)(rtContext);
    (void)(sphere);
    (void)(pos);

    // printf("\nsky_hit()\n");

    double z = ray->direction.z;
    if (z < 0) {
        return skyBottomColor;
    } else {
        return gradient(&skyBottomColor, &skyTopColor, 0, 1, z);
    }
}

static Color ground_hit(Scene *scene, Ray *ray, RTContext *rtContext, Sphere *sphere, Vector3 *pos)
{
    (void)(scene);      // Disable gcc -Wextra "unused parameter" errors.
    (void)(ray);
    (void)(rtContext);
    (void)(sphere);
    (void)(pos);

    // printf("\nground_hit()\n");

    return (Color)COLOR_GROUND;
}

static Color light_hit(Scene *scene, Ray *ray, RTContext *rtContext, Sphere *sphere, Vector3 *pos)
{
    (void)(scene);      // Disable gcc -Wextra "unused parameter" errors.
    (void)(ray);
    (void)(rtContext);
    (void)(pos);

    // printf("\nlight_hit()\n");

    MaterialDataLight *matData = sphere->matData;

    // printf("matData = %p\n", matData);
    // printf("matData->color .red = %f, .green = %f, .blue = %f", matData->color.red, matData->color.green, matData->color.blue);
    // exit(1);

    return matData->color;
}

static Color metal_hit(Scene *scene, Ray *ray, RTContext *rtContext, Sphere *sphere, Vector3 *pos)
{
    (void)(scene);      // Disable gcc -Wextra "unused parameter" errors.
    (void)(ray);
    (void)(rtContext);
    (void)(pos);

    // printf("\nlight_hit()\n");

    // A metal surface does a mirror-like reflection of incoming light, along the surface normal.

    Vector3 normal;
    calc_sphere_surface_normal(sphere, pos, &normal);

    Vector3 bouncedRayDirection;
    MaterialDataMetal *matData = sphere->matData;
    mirror_reflect(&ray->direction, &normal, matData->fuzziness, &bouncedRayDirection);

    Ray bouncedRay = (Ray){
        .origin = *pos,
        .direction = bouncedRayDirection,
    };

    Color bouncedRayIncomingColor;
    bool traceSuccess = ray_trace(rtContext, scene, &bouncedRay, &bouncedRayIncomingColor);

    // printf("traceSuccess = %d\n", traceSuccess);

    if (! traceSuccess) {
        // Could not find any incoming color (light), so just shade this pixel of the sphere with black.
        return (Color)COLOR_BLACK;
    }

    Color *sphereColor = &sphere->color;

    return (Color){
        .red    = sphereColor->red   * bouncedRayIncomingColor.red,
        .green  = sphereColor->green * bouncedRayIncomingColor.green,
        .blue   = sphereColor->blue  * bouncedRayIncomingColor.blue,
    };
}

static inline void mirror_reflect(Vector3 *incoming, Vector3 *normal, double fuzziness, Vector3 *reflected)
{
    // Mirror reflection ray direction is calculated by formula: reflected = incoming + 2b
    // Where b = normal * vector3_dot(-incoming, normal)
    //
    // Or, the same can be expressed as: reflected = incoming - (2 * normal * vector3_dot(incoming, normal))

    Vector3 b2 = *normal;
    vector3_multiply_length(&b2, 2 * vector3_dot(incoming, normal));

    vector3_subtract(incoming, &b2, reflected);

    if (fuzziness > DBL_EPSILON) {
        Vector3 randomPointInUnitSphere;
        random_point_in_unit_sphere(&randomPointInUnitSphere);
        vector3_multiply_length(&randomPointInUnitSphere, fuzziness);

        vector3_add_to(reflected, &randomPointInUnitSphere, reflected);
    }
}
