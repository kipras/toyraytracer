#ifndef __SPHERE_H__
#define __SPHERE_H__

typedef struct Sphere_s         Sphere;


#include <stdint.h>

#include "color.h"
#include "vector.h"
#include "material.h"


struct Sphere_s {
    Vector3         center;
    uint32_t        radius;
    Material       *material;
    void           *matData;
    Color           color;
};

#endif // __SPHERE_H__
