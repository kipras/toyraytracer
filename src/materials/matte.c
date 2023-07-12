#include "matte.h"
#include "../material.h"
#include "../ray_inline_fns.h"


static Color mat_matte_hit(Scene *scene, Ray *ray, RTContext *rtContext, Sphere *sphere, Vector3 *pos);


Material matMatte = {
    // .type = MT_Shaded,
    // .scatter = matte_scatter,
    .hit = mat_matte_hit,
};

static Color mat_matte_hit(Scene *scene, Ray *ray, RTContext *rtContext, Sphere *sphere, Vector3 *pos)
{
    (void)(ray);        // Disable gcc -Wextra "unused parameter" errors.

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

    return mat_trace_scattered_ray(scene, rtContext, sphere, pos, &bouncedRayDirection, true);
}
