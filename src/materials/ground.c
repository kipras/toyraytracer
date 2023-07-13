#include "../material.h"


static Color ground_hit(Scene *scene, Ray *ray, RTContext *rtContext, Sphere *sphere, Vector3 *pos);


Material matGround = {
    .hit = ground_hit,
};


static Color ground_hit(Scene *scene, Ray *ray, RTContext *rtContext, Sphere *sphere, Vector3 *pos)
{
    (void)(scene);      // Disable gcc -Wextra "unused parameter" errors.
    (void)(ray);
    (void)(rtContext);
    (void)(sphere);
    (void)(pos);

    return (Color)COLOR_GROUND;
}
