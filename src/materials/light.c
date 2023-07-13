#include "light.h"
#include "../material.h"
#include "../rtalloc.h"


static Color light_hit(Scene *scene, Ray *ray, RTContext *rtContext, Sphere *sphere, Vector3 *pos);


Material matLight = {
    .hit = light_hit,
};


Sphere * sphere_light_init(Sphere *sphere, Color color)
{
    sphere->material = &matLight;

    MaterialDataLight *matData = rtalloc(sizeof(MaterialDataLight));
    matData->color = color;
    sphere->matData = matData;

    return sphere;
}

static Color light_hit(Scene *scene, Ray *ray, RTContext *rtContext, Sphere *sphere, Vector3 *pos)
{
    (void)(scene);      // Disable gcc -Wextra "unused parameter" errors.
    (void)(ray);
    (void)(rtContext);
    (void)(pos);

    MaterialDataLight *matData = sphere->matData;

    return matData->color;
}

