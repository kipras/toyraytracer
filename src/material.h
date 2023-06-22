#ifndef __MATERIAL_H__
#define __MATERIAL_H__


typedef struct Material_s       Material;
typedef enum   MaterialType_e   MaterialType;


#include "ray.h"
#include "vector.h"


// enum MaterialType_e {
//     // MT_Matte = 1,       // Diffuse ? Lambert ?
//     // MT_Metal,           // ?? (Specular ??)
//     // MT_Glass,           // Refractive ? Dielectric ?
//     MT_Shaded,
//     MT_Sky,             // A special material for the sky sphere.
//     MT_Ground,          // A (temporary) special material for the ground sphere.
// };


struct Material_s {
    // MaterialType type;
    // void (*scatter)(Ray *rayIn, Ray *rayOut);
    Color (*hit)(Scene *scene, Ray *ray, Sphere *sphere, Vector3 *pos);
};


// A special shaded material (was used for initial testing).
extern Material matShaded;

// A special material for the sky sphere.
extern Material matSky;

// A (temporary) special material for the ground sphere.
extern Material matGround;

#endif // __MATERIAL_H__
