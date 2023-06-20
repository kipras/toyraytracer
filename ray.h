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
 * If `ray` hits `sphere` - returns true and sets `hitPoint` to the point on the `sphere` where `ray` hit it.
 * Otherwise returns false.
 */
bool ray_hits_sphere(Ray *ray, Sphere *sphere, Vector3 *hitPoint);


static inline void ray_point(Ray *ray, double dist, Vector3 *point)
{
    Vector3 rayDirAtDist = ray->direction;
    vector3_multiply_length(&rayDirAtDist, dist);

    vector3_add_to(&ray->origin, &rayDirAtDist, point);
}

#endif // __RAY_H__
