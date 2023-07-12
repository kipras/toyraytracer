#include "dielectric.h"
#include "../material.h"
#include "../ray_inline_fns.h"
#include "../rtalloc.h"


static Color dielectric_hit(Scene *scene, Ray *ray, RTContext *rtContext, Sphere *sphere, Vector3 *pos);
static inline bool does_ray_hit_from_sphere_inside(Vector3 *rayDir, Vector3 *sphereOutwardNormal);
static inline double schlicks_reflectance_approximation(double cosTheta, double refractionRatio);
static inline void refract(Vector3 *incoming, Vector3 *normal, double refractionRatio, double cosTheta, Vector3 *refracted);


Material matDielectric = {
    .hit = dielectric_hit,
};


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
        mat_mirror_reflect(&ray->direction, sphereRayHitNormal, 0.0, &scatteredRayDirection);
    } else {
        refract(&ray->direction, sphereRayHitNormal, refractionRatio, cosTheta, &scatteredRayDirection);
    }

    return mat_trace_scattered_ray(scene, rtContext, sphere, pos, &scatteredRayDirection, false);
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
