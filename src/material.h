#ifndef __MATERIAL_H__
#define __MATERIAL_H__

typedef struct Material_s                   Material;
// typedef enum   MaterialType_e               MaterialType;


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
     * Returns the color of the `sphere` at position `pos` in the `scene`.
     * Internally this function can call ray_trace() (depending on the material) to calculate incoming light from scattered rays.
     *
     * NOTE: this function can modify `ray` and `pos`.
     */
    Color (*hit)(Scene *scene, Ray *ray, RTContext *rtContext, Sphere *sphere, Vector3 *pos);
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

// A metal (specular highlights reflection) material.
extern Material matMetal;

// A dielectric (clear) material (e.g. water, glass).
extern Material matDielectric;


/**
 * Calculates reflection for an `incoming` ray, off of a mirror surface `normal`, and stores the reflected ray direction in `reflected`.
 *
 * If fuzziness > 0, then also applies fuzziness, which gives a "brushed" metal look. See MaterialDataMetal.fuzziness description.
 *
 * IMPORTANT: this function expects the surface `normal` vector to go in the opposite direction from the `incoming` ray direction vector.
 */
void mat_mirror_reflect(Vector3 *incoming, Vector3 *normal, double fuzziness, Vector3 *reflected);

/**
 * Given a direction of a ray that scattered after hitting a sphere - traces the scattered ray and returns the final color for the
 * **incoming** ray (meaning this also computes the shading between the color returned by the scattered ray and the color of the sphere
 * itself).
 *
 * NOTE: the incoming ray may have hit from outside the sphere or from inside the sphere. This function at the moment only properly handles
 * the case where the ray hit from outside the sphere, but should handle both cases (TODO).
 *
 * @param scene The scene.
 * @param rtContext The ray-tracing context for this ray.
 * @param sphere The sphere that was hit.
 * @param pos The position where the sphere was hit by the incoming ray. This will be used as the origin point for the scattered ray.
 * @param rayDirection The direction of the scattered ray.
 * @param attenuate A boolean, determining whether to use the color of the sphere when determining the final color or not. This will be
 *                  false for colorless see-through materials (e.g. glass) and true otherwise. This is a micro-optimisation to avoid
 *                  needlessly multiplying the traced scattered ray color by 1.0, for those materials.
 * @return Color The color to be returned for the **incoming** ray.
 */
Color mat_trace_scattered_ray(Scene *scene, RTContext *rtContext, Sphere *sphere, Vector3 *pos, Vector3 *rayDirection, bool attenuate);

#endif // __MATERIAL_H__
