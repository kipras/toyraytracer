#include "../material.h"
#include "../ray_inline_fns.h"


static Color shaded_hit(Scene *scene, Ray *ray, RTContext *rtContext, Sphere *sphere, Vector3 *pos);


Material matShaded = {
    // .type = MT_Shaded,
    // .scatter = matte_scatter,
    .hit = shaded_hit,
};


static Color shaded_hit(Scene *scene, Ray *ray, RTContext *rtContext, Sphere *sphere, Vector3 *pos)
{
    (void)(scene);      // Disable gcc -Wextra "unused parameter" errors.
    (void)(ray);
    (void)(rtContext);
    (void)(sphere);
    (void)(pos);

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

