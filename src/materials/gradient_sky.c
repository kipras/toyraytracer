#include "../material.h"


static Color mat_gradient_sky_hit(Scene *scene, Ray *ray, RTContext *rtContext, Sphere *sphere, Vector3 *pos);


Material matGradientSky = {
    // .type = MT_Sky,
    .hit = mat_gradient_sky_hit,
};


static Color skyTopColor = (Color)COLOR_GRADIENT_SKY_TOP;
static Color skyBottomColor = (Color)COLOR_GRADIENT_SKY_BOTTOM;

static Color mat_gradient_sky_hit(Scene *scene, Ray *ray, RTContext *rtContext, Sphere *sphere, Vector3 *pos)
{
    (void)(scene);      // Disable gcc -Wextra "unused parameter" errors.
    (void)(rtContext);
    (void)(sphere);
    (void)(pos);

    double z = ray->direction.z;
    if (z < 0) {
        return skyBottomColor;
    } else {
        return gradient(&skyBottomColor, &skyTopColor, 0, 1, z);
    }
}

