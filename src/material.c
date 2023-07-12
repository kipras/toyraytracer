#include <float.h>
#include <stdio.h>

#include "material.h"

#include "color.h"
#include "ray.h"
#include "rtalloc.h"
#include "rtmath.h"
#include "vector.h"


static Color matte_hit(Scene *scene, Ray *ray, RTContext *rtContext, Sphere *sphere, Vector3 *pos);
static Color shaded_hit(Scene *scene, Ray *ray, RTContext *rtContext, Sphere *sphere, Vector3 *pos);
static Color sky_hit(Scene *scene, Ray *ray, RTContext *rtContext, Sphere *sphere, Vector3 *pos);
static Color ground_hit(Scene *scene, Ray *ray, RTContext *rtContext, Sphere *sphere, Vector3 *pos);
static Color light_hit(Scene *scene, Ray *ray, RTContext *rtContext, Sphere *sphere, Vector3 *pos);
static Color metal_hit(Scene *scene, Ray *ray, RTContext *rtContext, Sphere *sphere, Vector3 *pos);
static inline void mirror_reflect(Vector3 *incoming, Vector3 *normal, double fuzziness, Vector3 *reflected);
static Color dielectric_hit(Scene *scene, Ray *ray, RTContext *rtContext, Sphere *sphere, Vector3 *pos);
static inline bool does_ray_hit_from_sphere_inside(Vector3 *rayDir, Vector3 *sphereOutwardNormal);
static inline double schlicks_reflectance_approximation(double cosTheta, double refractionRatio);
static inline void refract(Vector3 *incoming, Vector3 *normal, double refractionRatio, double cosTheta, Vector3 *refracted);
static inline Color _trace_scattered_ray(
    Scene *scene, RTContext *rtContext, Sphere *sphere, Vector3 *pos, Vector3 *rayDirection, bool attenuate);


Material matMatte = {
    // .type = MT_Shaded,
    // .scatter = matte_scatter,
    .hit = matte_hit,
};
Material matShaded = {
    // .type = MT_Shaded,
    // .scatter = matte_scatter,
    .hit = shaded_hit,
};
Material matGradientSky = {
    // .type = MT_Sky,
    .hit = sky_hit,
};
Material matGround = {
    // .type = MT_Ground,
    .hit = ground_hit,
};
Material matLight = {
    // .type = MT_Ground,
    .hit = light_hit,
};
Material matMetal = {
    .hit = metal_hit,
};
Material matDielectric = {
    .hit = dielectric_hit,
};


static Color matte_hit(Scene *scene, Ray *ray, RTContext *rtContext, Sphere *sphere, Vector3 *pos)
{
    (void)(ray);        // Disable gcc -Wextra "unused parameter" errors.

    Vector3 normal;
    calc_sphere_surface_normal(sphere, pos, &normal);

    // Generate a random point/vector for the scattered ray that will scatter from the `pos` (where the incoming ray
    // hits this surface). How we will generate the scattered ray depends on MATTE_DIFFUSE_TYPE.
    MatteDiffuseAlgo mda = MATTE_DIFFUSE_ALGO;
    Vector3 bouncedRayDirection;
    if (mda == MDA_randomVectorInHemisphere) {
        random_point_in_hemisphere(&bouncedRayDirection, &normal);
        vector3_to_unit(&bouncedRayDirection);
    } else {
        // MDA_randomVectorInUnitSphere or MDA_randomUnitVectorInUnitSphere
        Vector3 unitSphereRandomPoint;
        random_point_in_unit_sphere(&unitSphereRandomPoint);
        if (mda == MDA_randomUnitVectorInUnitSphere) {
            // Must convert to a unit vector, otherwise addition math will be wrong.
            vector3_to_unit(&unitSphereRandomPoint);
        }
        vector3_add_to(&normal, &unitSphereRandomPoint, &bouncedRayDirection);
        vector3_to_unit(&bouncedRayDirection);
    }

    return _trace_scattered_ray(scene, rtContext, sphere, pos, &bouncedRayDirection, true);
}

static Color shaded_hit(Scene *scene, Ray *ray, RTContext *rtContext, Sphere *sphere, Vector3 *pos)
{
    (void)(scene);      // Disable gcc -Wextra "unused parameter" errors.
    (void)(ray);
    (void)(rtContext);
    (void)(sphere);
    (void)(pos);

    // printf("\n shaded_hit() \n");

    // // @TODO: Implement me!
    // return (Color)COLOR_BLACK;

    // Ray scatteredRay;
    // minSphere->material->scatter(ray, &scatteredRay);

    // Calculate the sphere surface normal vector at `pos`.
    // I.e. a normalized (unit) vector from `sphere->center` to `pos`.
    Vector3 normal;
    calc_sphere_surface_normal(sphere, pos, &normal);

    return (Color){
        .red = floor((normal.x + 1) * 128),
        .green = floor((normal.y + 1) * 128),
        .blue = floor((normal.z + 1) * 128),
    };
}

static Color skyTopColor = (Color)COLOR_GRADIENT_SKY_TOP;
static Color skyBottomColor = (Color)COLOR_GRADIENT_SKY_BOTTOM;

static Color sky_hit(Scene *scene, Ray *ray, RTContext *rtContext, Sphere *sphere, Vector3 *pos)
{
    (void)(scene);      // Disable gcc -Wextra "unused parameter" errors.
    (void)(rtContext);
    (void)(sphere);
    (void)(pos);

    // printf("\n sky_hit() \n");

    double z = ray->direction.z;
    if (z < 0) {
        return skyBottomColor;
    } else {
        return gradient(&skyBottomColor, &skyTopColor, 0, 1, z);
    }
}

static Color ground_hit(Scene *scene, Ray *ray, RTContext *rtContext, Sphere *sphere, Vector3 *pos)
{
    (void)(scene);      // Disable gcc -Wextra "unused parameter" errors.
    (void)(ray);
    (void)(rtContext);
    (void)(sphere);
    (void)(pos);

    // printf("\n ground_hit() \n");

    return (Color)COLOR_GROUND;
}

static Color light_hit(Scene *scene, Ray *ray, RTContext *rtContext, Sphere *sphere, Vector3 *pos)
{
    (void)(scene);      // Disable gcc -Wextra "unused parameter" errors.
    (void)(ray);
    (void)(rtContext);
    (void)(pos);

    // printf("\n light_hit() \n");

    MaterialDataLight *matData = sphere->matData;

    // printf("matData = %p\n", matData);
    // printf("matData->color .red = %f, .green = %f, .blue = %f", matData->color.red, matData->color.green, matData->color.blue);
    // exit(1);

    return matData->color;
}

static Color metal_hit(Scene *scene, Ray *ray, RTContext *rtContext, Sphere *sphere, Vector3 *pos)
{
    // A metal surface does a mirror-like reflection of incoming light, along the surface normal.

    Vector3 normal;
    calc_sphere_surface_normal(sphere, pos, &normal);

    Vector3 bouncedRayDirection;
    MaterialDataMetal *matData = sphere->matData;
    mirror_reflect(&ray->direction, &normal, matData->fuzziness, &bouncedRayDirection);

    return _trace_scattered_ray(scene, rtContext, sphere, pos, &bouncedRayDirection, true);
}

/**
 * Calculates reflection for an `incoming` ray, off of a mirror surface `normal`, and stores the reflected ray direction in `reflected`.
 *
 * If fuzziness > 0, then also applies fuzziness, which gives a "brushed" metal look. See MaterialDataMetal.fuzziness description.
 *
 * IMPORTANT: this function expects the surface `normal` vector to go in the opposite direction from the `incoming` ray direction vector.
 */
static inline void mirror_reflect(Vector3 *incoming, Vector3 *normal, double fuzziness, Vector3 *reflected)
{
    // Mirror reflection ray direction is calculated by formula: reflected = incoming + 2b
    // Where b = normal * vector3_dot(-incoming, normal)
    //
    // Or, the same can be expressed as: reflected = incoming - (2 * normal * vector3_dot(incoming, normal))

    Vector3 b2 = *normal;
    vector3_multiply_length(&b2, 2 * vector3_dot(incoming, normal));

    vector3_subtract(incoming, &b2, reflected);

    if (fuzziness > DBL_EPSILON) {          // if (fuzziness > 0)
        // Produce a "fuzzy" reflection. This gives a "brushed" metal look (like christmas tree bubbles :) ).
        Vector3 randomPointInUnitSphere;
        random_point_in_unit_sphere(&randomPointInUnitSphere);
        vector3_multiply_length(&randomPointInUnitSphere, fuzziness);

        vector3_add_to(reflected, &randomPointInUnitSphere, reflected);

        vector3_to_unit(reflected);
    }
}

Sphere * sphere_dielectric_init(Sphere *sphere, double refractionIndex)
{
    sphere->material = &matDielectric;

    MaterialDataDielectric *matData = rtalloc(sizeof(MaterialDataDielectric));
    matData->refractionIndex = refractionIndex;
    matData->_refractionIndexInvBackToAir = 1.0 / refractionIndex;
    sphere->matData = matData;

    return sphere;
}

/**
 * Dielectric material.
 *
 * A dielectric surface is a clear (see-through) surface. When a ray of light hits a dielectric material - it can
 * reflect or it can "refract" (meaning it goes through the surface boundary, but its angle).
 *
 * Also note, that because a ray that hits a dielectric material can refract (meaning it can go inside the object) -
 * rays can also hit the object surface from _inside_ the object.
 *
 * But the rays of light may "refract", depending on "refractive indexes" ratio of
 * the two materials (the sphere and outside the sphere).
 *
 * IMPORTANT: it is important for this function that ray->direction is a **unit** vector !
 */
static Color dielectric_hit(Scene *scene, Ray *ray, RTContext *rtContext, Sphere *sphere, Vector3 *pos)
{
    Vector3 sphereSurfaceNormal;
    calc_sphere_surface_normal(sphere, pos, &sphereSurfaceNormal);

    bool isHitFromSphereInside = does_ray_hit_from_sphere_inside(&ray->direction, &sphereSurfaceNormal);

    Vector3 *sphereRayHitNormal;
    Vector3 sphereSurfaceNormalReversed;
    if (! isHitFromSphereInside) {
        sphereRayHitNormal = &sphereSurfaceNormal;
    } else {
        sphereSurfaceNormalReversed = sphereSurfaceNormal;
        vector3_multiply_length(&sphereSurfaceNormalReversed, -1.0);
        sphereRayHitNormal = &sphereSurfaceNormalReversed;
    }

    MaterialDataDielectric *matData = sphere->matData;
    double refractionRatio = isHitFromSphereInside ? matData->refractionIndex : matData->_refractionIndexInvBackToAir;

    // It is important for this calculation that `ray->direction` is a unit vector.
    // Otherwise we would have to divide the result of the dot product by the length of `ray->direction` to get the
    // cosine of the angle between them. (We don't have to divide by `normal`, because it is always a unit vector).
    double cosTheta = vector3_dot(&ray->direction, &sphereSurfaceNormal);
    if (! isHitFromSphereInside) {
        cosTheta = -cosTheta;
    }

    // The "Ray Tracing in One Weekend" book here also applied `cosTheta = fmin(cosTheta, 1.0)` (i.e. an 1.0 upper bound for `cosTheta`).
    // However, were not adding it, because it is not needed. `cosTheta` is a dot product between two unit vectors (i.e. an actual cosine
    // of the angle between them) so it cannot exceed 1.0 (except maybe for floating point inaccuracies/deviations). We've tested this and
    // `cosTheta` did not exceed 1.0 in our tests, so we're not adding this upper bound.
    //
    // If it were to exceed 1.0 somehow however - then `sinTheta` will be assigned a NaN value below, and subsequently `cannotRefract` will
    // be false, because `refractionRatio * sinTheta` will be NaN as well (and comparing NaN with any number always evaluates to false,
    // except when comparing with != operator (then it is always true)). So the outcome in that case is correct (`cannotRefract` should be
    // false, if `cosTheta == 1.0` (and so `sinTheta == 0.0`)) and so we don't need to add any additional checks/limits.

    double sinTheta = sqrt(1.0 - cosTheta*cosTheta);

    Vector3 scatteredRayDirection;
    bool cannotRefract = (refractionRatio * sinTheta) > 1.0;

    if (cannotRefract || schlicks_reflectance_approximation(cosTheta, refractionRatio) > random_double_0_1_exc()) {
        mirror_reflect(&ray->direction, sphereRayHitNormal, 0.0, &scatteredRayDirection);
    } else {
        refract(&ray->direction, sphereRayHitNormal, refractionRatio, cosTheta, &scatteredRayDirection);
    }

    return _trace_scattered_ray(scene, rtContext, sphere, pos, &scatteredRayDirection, false);
}

static inline bool does_ray_hit_from_sphere_inside(Vector3 *rayDir, Vector3 *sphereOutwardNormal)
{
    return vector3_dot(rayDir, sphereOutwardNormal) > 0.0;
}

static inline double schlicks_reflectance_approximation(double cosTheta, double refractionRatio)
{
    double r0root = ((double)(1.0 - refractionRatio)) / (1.0 + refractionRatio);
    double r0 = r0root*r0root;
    return r0 + ((1 - r0) * pow((1 - cosTheta), 5));
}

static inline void refract(Vector3 *incoming, Vector3 *normal, double refractionRatio, double cosTheta, Vector3 *refracted)
{
    Vector3 refrPerp;
    Vector3 normalMultipliedByCosTheta = *normal;
    vector3_multiply_length(&normalMultipliedByCosTheta, cosTheta);
    vector3_add_to(incoming, &normalMultipliedByCosTheta, &refrPerp);
    vector3_multiply_length(&refrPerp, refractionRatio);

    Vector3 refrPar = *normal;
    double refrPerpLen = vector3_length(&refrPerp);
    double refrParMultiplier = -sqrt(fabs(1.0 - (refrPerpLen*refrPerpLen)));
    vector3_multiply_length(&refrPar, refrParMultiplier);

    vector3_add_to(&refrPerp, &refrPar, refracted);

    vector3_to_unit(refracted);
}

/**
 * Given a direction of a ray that scattered after hitting a sphere - traces the scattered ray and returns the final color for the
 * **incoming** ray (meaning this also computes the shading between the color returned by the scattered ray and the color of the sphere
 * itself).
 *
 * NOTE: the incoming ray may have hit from outside the sphere or from inside the sphere. This function at the moment only properly handles
 * the case where the ray hit from outside the sphere, but should handle both cases (TODO).
 *
 * @TODO: handle the case where the incoming ray hits from inside the sphere.
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
static inline Color _trace_scattered_ray(
    Scene *scene, RTContext *rtContext, Sphere *sphere, Vector3 *pos, Vector3 *rayDirection, bool attenuate)
{
    Ray scatteredRay = (Ray){
        .origin = *pos,
        .direction = *rayDirection,
    };

    Color scatteredRayColor;
    bool traceSuccess = ray_trace(rtContext, scene, &scatteredRay, &scatteredRayColor);

    // printf("traceSuccess = %d\n", traceSuccess);

    if (! traceSuccess) {
        // Could not find any incoming color (light), so just shade this pixel of the sphere with black.
        return (Color)COLOR_BLACK;
    }

    if (attenuate) {
        Color *sphereColor = &sphere->color;
        scatteredRayColor.red *= sphereColor->red;
        scatteredRayColor.green *= sphereColor->green;
        scatteredRayColor.blue *= sphereColor->blue;
    }

    return scatteredRayColor;
}
