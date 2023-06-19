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
 * Returns a 3D coordinate (Point3) that is at `dist` distance along the vector.
 */
Vector3 * ray_point(Ray *ray, double dist);

/**
 * Returns `true` if `ray` hits `sphere`, otherwise returns `false`.
 */
bool ray_hit_sphere(Ray *ray, Sphere *sphere);

#endif // __RAY_H__
