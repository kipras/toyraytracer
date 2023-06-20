#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <math.h>

#include "main.h"


typedef struct Vector3_s        Vector3;


struct Vector3_s {
    // 2D plane coordinates.
    double x;
    double y;

    // Depth/height coordinate.
    double z;
};


// /**
//  * Allocates and returns a new Vector3.
//  */
// static inline Vector3 * vector3_alloc();

/**
 * Calculates and returns the length of given vector `v`.
 */
static inline double vector3_length(Vector3 *v);

/**
 * Normalizes given vector `v` to a unit vector, i.e. to a vector of length 1.
 */
static inline void vector3_to_unit(Vector3 *v);

/**
 * Adds two vectors `v1` and `v2` and stores the result in the `result` vector.
 */
static inline void vector3_add_to(Vector3 *v1, Vector3 *v2, Vector3 *result);

// /**
//  * Adds two given Vectors and returns a new Vector (which is the sum of them).
//  */
// static inline Vector3 * vector3_add(Vector3 *v1, Vector3 *v2);

/**
 * Subtracts the `subtrahend` vector from the `base` vector.
 */
static inline void vector3_subtract_from(Vector3 *base, Vector3 *subtrahend);

// /**
//  * Same as vector3_add(), but returns a Point3 instead of a Vector3.
//  */
// static inline Point3 * vector3_add_p(Vector3 *v1, Vector3 *v2);

/**
 * Produces a dot-product of the two given vectors (which is also the cos() of the angle between them).
 */
static inline double vector3_dot(Vector3 *v1, Vector3 *v2);

/**
 * Multiplies the given vector `v` by the given `multiplier` and stores the result in `v`.
 */
static inline void vector3_multiply_length(Vector3 *v, double multiplier);

/**
 * Divides the given vector `v` by the given `divisor` and stores the result in `v`.
 */
static inline void vector3_divide_length(Vector3 *v, double divisor);

/**
//  * Same as vector3_multiply_length(), but returns a Point3 instead of a Vector3.
//  */
// static inline Point3 * vector3_multiply_length_p(Vector3 *v, double multiplier);


extern void log_err(char *err);


// static inline Vector3 * vector3_alloc()
// {
//     Vector3 *vec;
//     vec = ralloc(sizeof(Vector3));
//     if (vec == NULL) {
//         log_err("Fatal error: could not allocate Vector3. Out of memory?");
//         exit(1);    // Exit program immediately, on memory allocation errors.
//     }
//     return vec;
// }

static inline double vector3_length(Vector3 *v)
{
    return sqrt(v->x*v->x + v->y*v->y + v->z*v->z);
}

static inline void vector3_to_unit(Vector3 *v)
{
    vector3_divide_length(v, vector3_length(v));
}

static inline void vector3_add_to(Vector3 *v1, Vector3 *v2, Vector3 *result)
{
    result->x = v1->x + v2->x;
    result->y = v1->y + v2->y;
    result->z = v1->z + v2->z;
}

// static inline Vector3 * vector3_add(Vector3 *v1, Vector3 *v2)
// {
//     Vector3 *rvec;
//     rvec = vector3_alloc();
//     rvec->x = v1->x + v2->x;
//     rvec->y = v1->y + v2->y;
//     rvec->z = v1->z + v2->z;
//     return rvec;
// }

static inline void vector3_subtract_from(Vector3 *base, Vector3 *subtrahend)
{
    base->x -= subtrahend->x;
    base->y -= subtrahend->y;
    base->z -= subtrahend->z;
}

// static inline Point3 * vector3_add_p(Vector3 *v1, Vector3 *v2)
// {
//     Point3 *p;
//     p = point3_alloc();
//     p->x = v1->x + v2->x;
//     p->y = v1->y + v2->y;
//     p->z = v1->z + v2->z;
//     return p;
// }

static inline double vector3_dot(Vector3 *v1, Vector3 *v2)
{
    return v1->x*v2->x + v1->y*v2->y + v1->z*v2->z;
}

static inline void vector3_multiply_length(Vector3 *v, double multiplier)
{
    v->x *= multiplier;
    v->y *= multiplier;
    v->z *= multiplier;
}

static inline void vector3_divide_length(Vector3 *v, double divisor)
{
    v->x /= divisor;
    v->y /= divisor;
    v->z /= divisor;
}

// static inline Vector3 * vector3_multiply_length(Vector3 *v, double multiplier)
// {
//     Vector3 *rvec = vector3_alloc();
//     rvec->x = v->x * multiplier;
//     rvec->y = v->y * multiplier;
//     rvec->z = v->z * multiplier;
//     return rvec;
// }

// static inline Point3 * vector3_multiply_length_p(Vector3 *v, double multiplier)
// {
//     Point3 *p = point3_alloc();
//     p->x = v->x * multiplier;
//     p->y = v->y * multiplier;
//     p->z = v->z * multiplier;
//     return p;
// }

#endif // __VECTOR_H__
