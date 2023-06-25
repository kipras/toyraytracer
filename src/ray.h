#ifndef __RAY_H__
#define __RAY_H__


#define RAY_BOUNCES_MAX     20

// A minimum distance a ray has to cover from origin to an object it hits.
// This is needed to prevent rays reflecting from an object hitting that same object, because of floating point precision issues.
#define RAY_DISTANCE_MIN    0.001


typedef struct Ray_s            Ray;
typedef struct RTContext_s      RTContext;


// #include "main.h"

#include "types.h"
#include "vector.h"


struct Ray_s {
    Vector3 origin;
    Vector3 direction;
};

// Ray tracing context.
struct RTContext_s {
    uint8_t bounces;
};


/**
 * Initializes the ray tracing context.
 */
static inline void ray_trace_context_init(RTContext *context);

/**
 * Sets `point` to a 3D coordinate, that is at `dist` distance along `ray`.
 */
static inline void ray_point(Ray *ray, double dist, Vector3 *point);

/**
 * Calculates sphere surface normal vector: a normalized (unit, i.e length 1) vector from `sphere->center` to `point` and stores it in
 * `normal`.
 */
static inline void calc_sphere_surface_normal(Sphere *sphere, Vector3 *point, Vector3 *normal);


/**
 * Traces `ray` through the `scene`.
 * IMPORTANT: ray->direction must be a **unit** vector (some materials expect the passed incoming ray to be a unit vector).
 * If the `ray` hits something (and is traced successfully) - then returns true and stores the resulting color in `color`.
 * Otherwise - returns false.
 */
bool ray_trace(RTContext *rtContext, Scene *scene, Ray *ray, Color *color);

/**
 * If `ray` hits `sphere` - the distance (>= 0) from the origin of the `ray` to the point on `sphere` where it hits.
 * Otherwise returns -1.
 */
double ray_distance_to_sphere(Ray *ray, Sphere *sphere);


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

#endif // __RAY_H__
