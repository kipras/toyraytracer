#include "metal.h"
#include "../material.h"
#include "../ray_inline_fns.h"


static Color mat_metal_hit(Scene *scene, Ray *ray, RTContext *rtContext, Sphere *sphere, Vector3 *pos);


Material matMetal = {
    .hit = mat_metal_hit,
};


static Color mat_metal_hit(Scene *scene, Ray *ray, RTContext *rtContext, Sphere *sphere, Vector3 *pos)
{
    // A metal surface does a mirror-like reflection of incoming light, along the surface normal.

    Vector3 normal;
    calc_sphere_surface_normal(sphere, pos, &normal);

    Vector3 bouncedRayDirection;
    MaterialDataMetal *matData = sphere->matData;
    mat_mirror_reflect(&ray->direction, &normal, matData->fuzziness, &bouncedRayDirection);

    return mat_trace_scattered_ray(scene, rtContext, sphere, pos, &bouncedRayDirection, true);
}
