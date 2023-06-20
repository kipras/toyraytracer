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

double ray_distance_to_sphere(Ray *ray, Sphere *sphere)
{
    // We determine if a ray hit a sphere using vector algebra.
    // We are solving this equation for t:
    // 0 = (t^2)*vector3_dot(ray->direction, ray->direction)
    //       + 2*t*vector3_dot(ray->direction, ray->origin - sphere->center)
    //       + vector3_dot(ray->origin - sphere->center, ray->origin - sphere->center)
    //       - (sphere->radius^2)
    //
    // This is a quadratic equation, i.e.: a*(x^2) + b*x + c = 0
    // Where:
    //      a: vector3_dot(ray->direction, ray->direction)
    //      b: 2*vector3_dot(ray->direction, ray->origin - sphere->center)
    //      c: vector3_dot(ray->origin - sphere->center, ray->origin - sphere->center) - (sphere->radius^2)
    //
    // Quadratic equations are solved using the quadratic formula:
    //      x = (-b +- sqrt(b^2 - 4*a*c)) / 2*a
    //
    // Note the +- and the sqrt() - this means there can be 2, 1 or 0 solutions to the equation, depending on the values of a, b, c.
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
    // The ray hits the sphere if the discriminant (the inside of sqrt()) is non-negative (there are 1 or 2 solutions to the equation).

    Vector3 ray_orig_and_sphere_diff = ray->origin; // Copy ray->origin into ray_orig_and_sphere_diff
    vector3_subtract_from(&ray_orig_and_sphere_diff, &sphere->center);

    double a = vector3_dot(&ray->direction, &ray->direction);
    double b = 2*vector3_dot(&ray->direction, &ray_orig_and_sphere_diff);
    double c = vector3_dot(&ray_orig_and_sphere_diff, &ray_orig_and_sphere_diff) - pow(sphere->radius, 2);

    double discriminant = pow(b, 2) - (4*a*c);

    if (discriminant < 0) {
        return -1;
    }

    // Ray hits the sphere. We want to return the closer (out of the two possible points) where ray hits the sphere.
    // To do that we use - out of the two (+-) solutions (to get the smaller value).
    return (-b - sqrt(discriminant)) / (2*a);
}
