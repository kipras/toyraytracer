#ifndef __RAY_H__
#define __RAY_H__

#include "main.h"

#include "vector.h"


#define RAY_BOUNCES_MAX     20


typedef struct Ray_s            Ray;


struct Ray_s {
    Vector3 origin;
    Vector3 direction;
};


/**
 * Sets `point` to a 3D coordinate, that is at `dist` distance along `ray`.
 */
static inline void ray_point(Ray *ray, double dist, Vector3 *point);

/**
 * Traces `ray` through the `scene` and returns the resulting color for the pixel from which the ray originated on.
 */
Color ray_trace(Scene *scene, Ray *ray);

/**
 * If `ray` hits `sphere` - the distance (>= 0) from the origin of the `ray` to the point on `sphere` where it hits.
 * Otherwise returns -1.
 */
double ray_distance_to_sphere(Ray *ray, Sphere *sphere);


static inline void ray_point(Ray *ray, double dist, Vector3 *point)
{
    Vector3 rayDirAtDist = ray->direction;
    vector3_multiply_length(&rayDirAtDist, dist);

    vector3_add_to(&ray->origin, &rayDirAtDist, point);
}

#endif // __RAY_H__
