#ifndef __MATERIAL_H__
#define __MATERIAL_H__


typedef struct Material_s                   Material;
typedef struct MaterialDataLight_s          MaterialDataLight;
typedef struct MaterialDataMetal_s          MaterialDataMetal;
typedef struct MaterialDataDielectric_s     MaterialDataDielectric;
// typedef enum   MaterialType_e               MaterialType;
typedef enum   MatteDiffuseAlgo_e           MatteDiffuseAlgo;


#include "ray.h"
#include "types.h"
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
     * Returns the color of the `sphere` at position `pos` in the `scene`.
     * Internally this function can call ray_trace() (depending on the material) to calculate incoming light from scattered rays.
     *
     * NOTE: this function can modify `ray` and `pos`.
     */
    Color (*hit)(Scene *scene, Ray *ray, RTContext *rtContext, Sphere *sphere, Vector3 *pos);
};

struct MaterialDataLight_s {
    // Here `color` must be: <light_color> * <luminosity>.
    // Where <light_color> is the standard 24bit Color type.
    // I'm not sure how to define the units for <luminosity> though. You can imagine that when <luminosity> = 1, then this is the same
    // as a matte object. Experiment with this to get the right value.
    Color color;
};

struct MaterialDataMetal_s {
    // The fuzziness of the reflected rays. Should be set to >= 0.
    // If set to 0 (no fuzziness) - then the material is perfect glass (i.e. produces perfect reflections).
    // The higher this value - the more the material behaves like a matte material (where reflected rays scatter in any direction) and less
    // like a metal material (where rays reflect with mirror reflections).
    // I.e. it gives a "brushed" metal look (like the one of christmas tree bubbles).
    double fuzziness;
};

struct MaterialDataDielectric_s {
    // The "refraction index" of the specific dielectric material. Different materials have different indexes.
    // Typically air = 1.0, glass = 1.3–1.7, diamond = 2.4 (as stated by the "Ray tracing in one weekend" book).
    double refractionIndex;

    // A pre-computed inverted refraction index (i.e. for when a ray goes from material back to air).
    // _refractionIndexInvBackToAir = 1.0 / refractionIndex
    double _refractionIndexInvBackToAir;
};


enum MatteDiffuseAlgo_e {
    MDA_randomVectorInUnitSphere = 1,
    MDA_randomUnitVectorInUnitSphere,
    MDA_randomVectorInHemisphere,
};

#define MATTE_DIFFUSE_ALGO      MDA_randomUnitVectorInUnitSphere



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

// A metal (specular highlights reflection) material.
extern Material matMetal;

// A dielectric (clear) material (e.g. water, glass).
extern Material matDielectric;


/**
 * Initializes `sphere` as a dielectric sphere. Returns the same `sphere` pointer (this is useful for chaining function calls).
 */
Sphere * sphere_dielectric_init(Sphere *sphere, double refractionIndex);


#define MAT_GLASS_REFRACTION_INDEX      1.5
static inline Sphere * sphere_glass_init(Sphere *sphereInitData)
{
    return sphere_dielectric_init(sphereInitData, MAT_GLASS_REFRACTION_INDEX);
}

#endif // __MATERIAL_H__
