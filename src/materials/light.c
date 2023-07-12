#include "light.h"
#include "../material.h"


static Color mat_light_hit(Scene *scene, Ray *ray, RTContext *rtContext, Sphere *sphere, Vector3 *pos);


Material matLight = {
    // .type = MT_Ground,
    .hit = mat_light_hit,
};


static Color mat_light_hit(Scene *scene, Ray *ray, RTContext *rtContext, Sphere *sphere, Vector3 *pos)
{
    (void)(scene);      // Disable gcc -Wextra "unused parameter" errors.
    (void)(ray);
    (void)(rtContext);
    (void)(pos);

    MaterialDataLight *matData = sphere->matData;

    return matData->color;
}

