#ifndef __MATERIAL_H__
#define __MATERIAL_H__


typedef struct Material_s           Material;
typedef struct MaterialLightData_s  MaterialLightData;
typedef enum   MaterialType_e       MaterialType;


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

    /**
     * NOTE: this function can modify `ray` and `pos`.
     */
    Color32 (*hit)(Scene *scene, Ray *ray, RTContext *rtContext, Sphere *sphere, Vector3 *pos);
};

struct MaterialLightData_s {
    // Here `color` must be: <light_color> * <luminosity>.
    // Where <light_color> is the standard 24bit Color type.
    // I'm not sure how to define the units for <luminosity> though. You can imagine that when <luminosity> = 1, then then this is the same
    // as a matte object. Experiment to get the right value.
    Color32 color;
};


// "Matte" material (uses diffuse/Lambertian reflection).
extern Material matMatte;

// A special shaded material (was used for initial testing).
extern Material matShaded;

// A special material for the sky sphere, where the sky sphere is colored in a gradient from light blue at the top to white at the middle.
// (see COLOR_GRADIENT_SKY_TOP, COLOR_GRADIENT_SKY_BOTTOM). Everything below middle (the bottom half) is also white.
extern Material matGradientSky;

// A (temporary) special material for the ground sphere.
extern Material matGround;

// A material that simply emits a specific color at all times (rays don't scatter off of it).
extern Material matLight;

#endif // __MATERIAL_H__
