#include "material.h"

#include "ray.h"


static void matte_scatter(Ray *ray_in, Ray *ray_out);


Material matte = {
    .type = MT_Matte,
    .scatter = matte_scatter,
};


static void matte_scatter(Ray *ray_in, Ray *ray_out)
{
    (void)(ray_in);
    (void)(ray_out);

    // @TODO: Implement me!
}
