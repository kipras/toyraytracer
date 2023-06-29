#ifndef __TYPES_H__
#define __TYPES_H__


#define SCENE_SPHERES_MAX   20


typedef struct Sphere_s         Sphere;
typedef struct Scene_s          Scene;


#include "color.h"
#include "material.h"
#include "ray.h"
#include "vector.h"


struct Sphere_s {
    Vector3         center;
    uint32_t        radius;
    Material       *material;
    void           *matData;
    Color           color;
};

struct Scene_s {
    Sphere          spheres[SCENE_SPHERES_MAX];
    uint32_t        spheresLength;
};

#endif // __TYPES_H__
