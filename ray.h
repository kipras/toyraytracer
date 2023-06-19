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


Vector3 * ray_point(Ray *ray, double dist);

#endif // __RAY_H__
