#ifndef __DIELECTRIC_H__
#define __DIELECTRIC_H__

typedef struct MaterialDataDielectric_s     MaterialDataDielectric;


#include "../types.h"


struct MaterialDataDielectric_s {
    // The "refraction index" of the specific dielectric material. Different materials have different indexes.
    // Typically air = 1.0, glass = 1.3–1.7, diamond = 2.4 (as stated by the "Ray tracing in one weekend" book).
    double refractionIndex;

    // A pre-computed inverted refraction index (i.e. for when a ray goes from material back to air).
    // _refractionIndexInvBackToAir = 1.0 / refractionIndex
    double _refractionIndexInvBackToAir;
};


/**
 * Initializes `sphere` as a dielectric sphere. Returns the same `sphere` pointer (this is useful for chaining function calls).
 */
Sphere * sphere_dielectric_init(Sphere *sphere, double refractionIndex);


#define MAT_GLASS_REFRACTION_INDEX      1.5
static inline Sphere * sphere_glass_init(Sphere *sphereInitData)
{
    return sphere_dielectric_init(sphereInitData, MAT_GLASS_REFRACTION_INDEX);
}

#endif // __DIELECTRIC_H__
