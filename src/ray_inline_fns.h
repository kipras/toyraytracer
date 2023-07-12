#ifndef __RAY_INLINE_FNS_H__
#define __RAY_INLINE_FNS_H__

#include "ray.h"


/**
 * Initializes the ray tracing context.
 */
static inline void ray_trace_context_init(RTContext *context);

/**
 * Sets `point` to a 3D coordinate, that is at `dist` distance along `ray`.
 * IMPORTANT: this requires ray->direction to be a unit vector.
 */
static inline void ray_point(Ray *ray, double dist, Vector3 *point);

/**
 * Calculates sphere surface normal vector: a normalized (unit, i.e length 1) vector from `sphere->center` to `point` and stores it in
 * `normal`.
 */
static inline void calc_sphere_surface_normal(Sphere *sphere, Vector3 *point, Vector3 *normal);


static inline void ray_trace_context_init(RTContext *context)
{
    context->bounces = 0;
}

static inline void ray_point(Ray *ray, double dist, Vector3 *point)
{
    Vector3 rayDirAtDist = ray->direction;
    vector3_multiply_length(&rayDirAtDist, dist);

    vector3_add_to(&ray->origin, &rayDirAtDist, point);
}

static inline void calc_sphere_surface_normal(Sphere *sphere, Vector3 *point, Vector3 *normal)
{
    *normal = *point;
    vector3_subtract_from(normal, &sphere->center);
    vector3_to_unit(normal);
}

#endif // __RAY_INLINE_FNS_H__
