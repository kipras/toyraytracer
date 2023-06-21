#ifndef __VECTOR_H__
#define __VECTOR_H__

// Need _USE_MATH_DEFINES to have <math.h> export the M_PI constant.
#define _USE_MATH_DEFINES
#include <math.h>

#include <stdbool.h>


typedef struct Vector3_s        Vector3;


#include "random.h"


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

/**
 * Generates a random point within a unit sphere (a sphere of radius 1), that is centered at [0, 0, 0].
 */
static inline void random_point_in_unit_sphere(Vector3 *point);

/**
 * Generates a random point within a unit sphere (a sphere of radius 1), that is centered at [0, 0, 0], using random attempts algorithm.
 * The generated point _can_ be on the border of the sphere (and in that case when expressed as a vector it will be a unit vector),
 * but most likely it will be closer to the sphere center.
 * The length of the generated point (as a vector) will be (0, 1].
 */
static inline void random_point_in_unit_sphere__random_algo(Vector3 *point);

/**
 * Generates a random point within a unit sphere (a sphere of radius 1), that is centered at [0, 0, 0], using a trigonometric algorithm.
 * This generates a point that is always a unit vector (is of length 1).
 */
static inline void random_point_in_unit_sphere__trigonometric_algo(Vector3 *point);


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


static inline void random_point_in_unit_sphere(Vector3 *point)
{
    // Checked the performance of these algos - it is basically the same. The trigonometric one is just slightly faster
    // (on a 3rd generation Intel Core i7 (Ivy Bridge)), so using that one.
    // It also has the benefit of always producing unit-length vectors.

    // // Note that random algo generates a _unit or smaller_ vector.
    // random_point_in_unit_sphere__random_algo(point);

    // Note that trigonometric algo always generates a unit vector.
    random_point_in_unit_sphere__trigonometric_algo(point);
}

static inline void random_point_in_unit_sphere__random_algo(Vector3 *point)
{
    // The random algorithm keeps trying to generate a point (where each coordinate is between [-1, 1]), until it generates one that is
    // within a unit sphere (a sphere of radius 1), that is centered at [0, 0, 0].
    // A point is within such a unit sphere, when it (expressed as a vector) is up to the length of a unit vector, but not longer
    // (i.e. it's length is up to (and including) 1).
    // Note that we mean _inside_ the sphere. That means that the point _can_ be on the border of the sphere (and in that case when
    // expressed as a vector it will be a unit vector), but most likely it will be closer to the sphere center.
    //
    // The idea here is that when we generate a point where each coordinate is between [-1, 1] - we end up generating points within a
    // _cube_ (of dimensions [2, 2, 2], centered at [0, 0, 0]). A unit sphere (centered at [0, 0, 0]) would be smaller than this cube.
    // Meaning, that sometimes such a generated point is inside the unit sphere and sometimes it is outside it. So we just keep generating
    // those points until we get one that is inside it.
    //
    // Note that the length of the generated point (as a vector) will be (0, 1].

    while (true) {
        *point = (Vector3){.x = random_double_inc(-1, 1), .y = random_double_inc(-1, 1), .z = random_double_inc(-1, 1)};
        double len = vector3_length(point);
        if (len != 0 && len <= 1.0) {
            break;
        }
    }
}

static inline void random_point_in_unit_sphere__trigonometric_algo(Vector3 *point)
{
    // This generates a point that is always a unit vector (is of length 1).

    double horAngle = random_double_exc(0, 2*M_PI);  // Horizontal angle (within a circle), in radians.
    double vertAngle = random_double_exc(0, M_PI);   // Vertical angle (within a half-circle), in radians.

    point->x = cos(horAngle);
    point->y = sin(horAngle);
    point->z = cos(vertAngle);
}

#endif // __VECTOR_H__
