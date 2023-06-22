#include "material.h"

#include "color.h"
#include "ray.h"
#include "vector.h"


// static void matte_scatter(Ray *ray_in, Ray *ray_out);
static Color shaded_hit(Scene *scene, Ray *ray, Sphere *sphere, Vector3 *pos);
static Color sky_hit(Scene *scene, Ray *ray, Sphere *sphere, Vector3 *pos);
static Color ground_hit(Scene *scene, Ray *ray, Sphere *sphere, Vector3 *pos);


Material matShaded = {
    // .type = MT_Shaded,
    // .scatter = matte_scatter,
    .hit = shaded_hit,
};
Material matSky = {
    // .type = MT_Sky,
    .hit = sky_hit,
};
Material matGround = {
    // .type = MT_Ground,
    .hit = ground_hit,
};


// static void matte_scatter(Ray *rayIn, Ray *rayOut)
// {
//     (void)(rayIn);
//     (void)(rayOut);

//     // @TODO: Implement me!
// }

static Color shaded_hit(Scene *scene, Ray *ray, Sphere *sphere, Vector3 *pos)
{
    (void)(scene);
    (void)(ray);
    (void)(sphere);
    (void)(pos);

    // // @TODO: Implement me!
    // return (Color)COLOR_BLACK;

    // Ray scatteredRay;
    // minSphere->material->scatter(ray, &scatteredRay);

    // Calculate the sphere surface normal vector at `pos`.
    // I.e. a normalized (unit) vector from `sphere->center` to `pos`.
    Vector3 *hpNormal = pos;
    vector3_subtract_from(hpNormal, &sphere->center);
    vector3_to_unit(hpNormal);

    return (Color){
        .red = floor((hpNormal->x + 1) * 128),
        .green = floor((hpNormal->y + 1) * 128),
        .blue = floor((hpNormal->z + 1) * 128),
    };
}

static Color skyTopColor = (Color)COLOR_SKY_TOP;
static Color skyBottomColor = (Color)COLOR_SKY_BOTTOM;

static Color sky_hit(Scene *scene, Ray *ray, Sphere *sphere, Vector3 *pos)
{
    (void)(scene);
    (void)(sphere);
    (void)(pos);

    double y = ray->direction.y;
    if (y < 0) {
        return skyBottomColor;
    } else {
        return gradient(&skyBottomColor, &skyTopColor, 0, 1, ray->direction.y);
    }
}

static Color ground_hit(Scene *scene, Ray *ray, Sphere *sphere, Vector3 *pos)
{
    (void)(scene);
    (void)(ray);
    (void)(sphere);
    (void)(pos);

    return (Color)COLOR_GROUND;
}
