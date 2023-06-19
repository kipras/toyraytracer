#include <math.h>
#include <stdio.h>

#include "main.h"

#include "ray.h"
#include "vector.h"


Vector3 * ray_point(Ray *ray, double dist)
{
    return vector3_add(&ray->origin, vector3_multiply_length(&ray->direction, dist));

    // return vector3_add_p(&ray->origin, vector3_multiply_length(&ray->direction, dist));
}

bool ray_hit_sphere(Ray *ray, Sphere *sphere)
{
    // We determine if a ray hit a sphere using vector algebra.
    // We are solving for the equation:
    // 0 = (t^2)*vector3_dot(ray->direction, ray->direction)
    //       + 2*t*vector3_dot(ray->direction, ray->origin - sphere->center)
    //       + vector3_dot(ray->origin - sphere->center, ray->origin - sphere->center)
    //       - (sphere->radius^2)
    //
    // After solving this equation for t using the quadratic formula, we get:
    // t = (
    //          -(2*vector3_dot(ray->direction, ray->origin - sphere->center))
    //          +- sqrt(
    //                      (2*vector3_dot(ray->direction, ray->origin - sphere->center))^2
    //                    - (4*vector3_dot(ray->direction, ray->direction)
    //                        * (   vector3_dot(ray->origin - sphere->center, ray->origin - sphere->center)
    //                            - (sphere->radius^2))))
    //     ) / (2*vector3_dot(ray->direction, ray->direction))
    //
    // The ray hits the sphere if the discriminant (the inside of sqrt()) is non-negative.

    Vector3 ray_orig_and_sphere_diff = ray->origin; // Copy ray->origin into ray_orig_and_sphere_diff
    vector3_subtract_from(&ray_orig_and_sphere_diff, &sphere->center);

    double discriminant = \
            pow((2*vector3_dot(&ray->direction, &ray_orig_and_sphere_diff)), 2) \
          - (4 * vector3_dot(&ray->direction, &ray->direction) \
               * (   vector3_dot(&ray_orig_and_sphere_diff, &ray_orig_and_sphere_diff) \
                   - pow(sphere->radius, 2)));

    return discriminant >= 0;
}
