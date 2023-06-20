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
 * If `ray` hits `sphere` - returns the distance from `ray` to `sphere` (i.e. to the point where `ray` hits `sphere` surface), which will
 * be a non-negative value.
 * If `ray` doesn't hit `sphere` - returns -1.
 */
double ray_distance_to_sphere(Ray *ray, Sphere *sphere);

#endif // __RAY_H__
