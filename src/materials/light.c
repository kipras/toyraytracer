#include "light.h"
#include "../material.h"


static Color light_hit(Scene *scene, Ray *ray, RTContext *rtContext, Sphere *sphere, Vector3 *pos);


Material matLight = {
    // .type = MT_Ground,
    .hit = light_hit,
};


static Color light_hit(Scene *scene, Ray *ray, RTContext *rtContext, Sphere *sphere, Vector3 *pos)
{
    (void)(scene);      // Disable gcc -Wextra "unused parameter" errors.
    (void)(ray);
    (void)(rtContext);
    (void)(pos);

    MaterialDataLight *matData = sphere->matData;

    return matData->color;
}

