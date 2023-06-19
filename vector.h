#ifndef __VECTOR_H__
#define __VECTOR_H__

#include "main.h"


inline Vector3 * vector3_alloc();

/**
 * Adds the `addition` vector to the `base` vector.
 */
inline void vector3_add_to(Vector3 base, Vector3 addition);

/**
 * Adds two given Vectors and returns a new Vector (which is the sum of them) or NULL if failed to allocate a new Vector.
 */
inline Vector3 * vector3_add(Vector3 *v1, Vector3 *v2);

/**
 * Multiplies the given vector by the given multiplier and returns the result as a new Vector.
 */
inline Vector3 * vector3_multiply_length(Vector3 *v, double multiplier);


inline Vector3 * vector3_alloc()
{
    Vector3 *vec;
    vec = ralloc(sizeof(Vector3));
    if (vec == NULL) {
        exit(1);    // Exit program immediately, on memory allocation errors.
    }
    return vec;
}

inline void vector3_add_to(Vector3 base, Vector3 addition)
{
    base.x += addition.x;
    base.y += addition.y;
    base.z += addition.z;
}


inline Vector3 * vector3_add(Vector3 *v1, Vector3 *v2)
{
    Vector3 *rvec;
    rvec = vector3_alloc();
    rvec->x = v1->x + v2->x;
    rvec->y = v1->y + v2->y;
    rvec->z = v1->z + v2->z;
    return rvec;
}

inline Vector3 * vector3_multiply_length(Vector3 *v, double multiplier)
{
    Vector3 *rvec;
    rvec = vector3_alloc();
    rvec->x = v->x * multiplier;
    rvec->y = v->y * multiplier;
    rvec->z = v->z * multiplier;
    return rvec;
}

#endif // __VECTOR_H__
