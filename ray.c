#include "main.h"

#include "ray.h"
#include "vector.h"


Vector3 * ray_point(Ray *ray, double dist)
{
    return vector3_add(&ray->origin, vector3_multiply_length(&ray->direction, dist));
}
