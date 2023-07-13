#ifndef __RAY_H__
#define __RAY_H__

#include <stdbool.h>
#include <stdint.h>


#define RAY_BOUNCES_MAX     20

// A minimum distance a ray has to cover from origin to an object it hits.
// This is needed to prevent rays reflecting from an object hitting that same object, because of floating point precision issues.
#define RAY_DISTANCE_MIN    0.001


typedef struct Ray_s            Ray;
typedef struct RTContext_s      RTContext;


#include "scene.h"
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
 * Traces `ray` through the `scene`.
 *
 * If the `ray` hits something (and is traced successfully) - then returns true and stores the resulting color in `color`.
 * Otherwise - returns false.
 *
 * IMPORTANT: ray->direction must be a **unit** vector (some materials expect the passed incoming ray to be a unit vector).
 *
 * NOTE: the ray may hit a sphere from outside of it or from inside it (e.g. for the sky sphere or if it is a ray that refracted off the
 * surface of the sphere to inside the sphere). This function handles both cases.
 */
bool ray_trace(RTContext *rtContext, Scene *scene, Ray *ray, Color *color);

#endif // __RAY_H__
