#ifndef __VECTOR_H__
#define __VECTOR_H__

#include "main.h"


typedef struct Vector3_s        Vector3;


struct Vector3_s {
    // 2D plane coordinates.
    double x;
    double y;

    // Depth/height coordinate.
    double z;
};


/**
 * Allocates and returns a new Vector3.
 */
static inline Vector3 * vector3_alloc();

/**
 * Adds the `addition` vector to the `base` vector.
 */
static inline void vector3_add_to(Vector3 *base, Vector3 *addition);

/**
 * Adds two given Vectors and returns a new Vector (which is the sum of them).
 */
static inline Vector3 * vector3_add(Vector3 *v1, Vector3 *v2);

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
 * Multiplies the given vector by the given multiplier and returns the result as a new Vector.
 */
static inline Vector3 * vector3_multiply_length(Vector3 *v, double multiplier);

/**
//  * Same as vector3_multiply_length(), but returns a Point3 instead of a Vector3.
//  */
// static inline Point3 * vector3_multiply_length_p(Vector3 *v, double multiplier);


extern void log_err(char *err);


static inline Vector3 * vector3_alloc()
{
    Vector3 *vec;
    vec = ralloc(sizeof(Vector3));
    if (vec == NULL) {
        log_err("Fatal error: could not allocate Vector3. Out of memory?");
        exit(1);    // Exit program immediately, on memory allocation errors.
    }
    return vec;
}

static inline void vector3_add_to(Vector3 *base, Vector3 *addition)
{
    base->x += addition->x;
    base->y += addition->y;
    base->z += addition->z;
}

static inline Vector3 * vector3_add(Vector3 *v1, Vector3 *v2)
{
    Vector3 *rvec;
    rvec = vector3_alloc();
    rvec->x = v1->x + v2->x;
    rvec->y = v1->y + v2->y;
    rvec->z = v1->z + v2->z;
    return rvec;
}

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

static inline Vector3 * vector3_multiply_length(Vector3 *v, double multiplier)
{
    Vector3 *rvec = vector3_alloc();
    rvec->x = v->x * multiplier;
    rvec->y = v->y * multiplier;
    rvec->z = v->z * multiplier;
    return rvec;
}

// static inline Point3 * vector3_multiply_length_p(Vector3 *v, double multiplier)
// {
//     Point3 *p = point3_alloc();
//     p->x = v->x * multiplier;
//     p->y = v->y * multiplier;
//     p->z = v->z * multiplier;
//     return p;
// }

#endif // __VECTOR_H__
