#include <float.h>

#include "material.h"


void mat_mirror_reflect(Vector3 *incoming, Vector3 *normal, double fuzziness, Vector3 *reflected)
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

Color mat_trace_scattered_ray(
    Scene *scene, RTContext *rtContext, Sphere *sphere, Vector3 *pos, Vector3 *rayDirection, bool attenuate)
{
    Ray scatteredRay = (Ray){
        .origin = *pos,
        .direction = *rayDirection,
    };

    Color scatteredRayColor;
    bool traceSuccess = ray_trace(rtContext, scene, &scatteredRay, &scatteredRayColor);

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
