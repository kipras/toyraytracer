#include <stdint.h>
#include <stdio.h>
#include <float.h>

#include "ray.h"
#include "ray_inline_fns.h"
#include "vector.h"


static double ray_distance_to_sphere(Ray *ray, Sphere *sphere);


bool ray_trace(RTContext *rtContext, Scene *scene, Ray *ray, Color *color)
{
    if (rtContext->bounces >= RAY_BOUNCES_MAX) {
        return false;
    }

    rtContext->bounces++;

    // Find the closest sphere that `ray` hits (if any) and store it in `minSphere` and the distance to it in `minDist`.
    bool hitSomething = false;
    double minDist = DBL_MAX;
    Sphere *minSphere;
    Sphere *sphereList = scene->spheres;
    for (uint32_t i = 0; i < scene->spheresLength; i++) {
        Sphere *sphere = &sphereList[i];
        double dist = ray_distance_to_sphere(ray, sphere);
        if (dist >= RAY_DISTANCE_MIN) {
            hitSomething = true;
            if (dist < minDist) {
                minDist = dist;
                minSphere = sphere;
            }
        }
    }

    if (! hitSomething) {
        // When we could not hit anything - return the environment's ambient color (darkness).
        // If we would want ambient lighting (i.e. lighting coming from everywhere) - then change this to that light's color.
        *color = (Color)COLOR_BLACK;
        return false;
    }

    // Set `hitPoint` to the point on `minSphere` where `ray` hits.
    Vector3 hitPoint;
    ray_point(ray, minDist, &hitPoint);

    *color = minSphere->material->hit(scene, ray, rtContext, minSphere, &hitPoint);

    return true;
}

/**
 * If `ray` hits `sphere` - returns the distance (>= 0) from the origin of the `ray` to the point on `sphere` where it hits.
 * Otherwise returns a negative value.
 */
static double ray_distance_to_sphere(Ray *ray, Sphere *sphere)
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
    //      x = (-b +- sqrt(b^2 - 4*a  *c)) / 2*a
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
    // A line drawn through the ray hits the sphere if the discriminant (the inside of sqrt()) is non-negative (there are 1 or 2 solutions
    // to the equation).

    Vector3 ray_orig_and_sphere_diff = ray->origin; // Copy ray->origin into ray_orig_and_sphere_diff
    vector3_subtract_from(&ray_orig_and_sphere_diff, &sphere->center);

    double a = vector3_dot(&ray->direction, &ray->direction);
    double b = 2*vector3_dot(&ray->direction, &ray_orig_and_sphere_diff);
    double c = vector3_dot(&ray_orig_and_sphere_diff, &ray_orig_and_sphere_diff) - (sphere->radius*sphere->radius);

    double discriminant = (b*b) - (4*a*c);

    if (discriminant < 0) {
        // If the discriminant is negative - then a line drawn through the ray doesn't intersect with the sphere at all.
        // That is, the ray doesn't intersect with the sphere and if we would reverse the ray (make it go in the opposite direction) - it
        // would also not intersect with the sphere.
        return -1;
    }

    // Since the discriminant is positive - that means that the ray _can_ have one or two intersections with the sphere.
    //
    // If the ray hits the sphere - then we want to return the closer (out of the two possible points) where ray hits the sphere.
    // To do that we use - out of the two (+-) solutions (to get the smaller value).
    //
    // It is also possible that the ray goes in the opposite direction (away) from the sphere. In that case - both solutions will
    // yield negative results. We can return either of the results in that case (a negative return value means that the ray did not hit the
    // sphere).
    //
    // There is also a nuance, regarding the sky sphere and dielectric materials (that refract some rays that hit the sphere to inside the
    // sphere) that rays can also hit from _inside_ of the sphere. In that case: sqrt(discriminant) > -b. So the smaller value is negative,
    // but the larger value is positive. So we do an additional check for this, and return the larger value in this case.
    //
    // Also, note that since refacted rays can bounce off (scatter) from sphere boundary to inside the sphere (for dielectric (e.g. glass)
    // spheres) - in that case it is important to return the second of the two solutions, not the first one. The first one will be around
    // 0.0, but because of floating point inaccuracies/deviations - we compare against RAY_DISTANCE_MIN instead (this is important,
    // otherwise refracted rays of dielectric spheres will be rendered incorrectly).
    //
    // TL;DR: we return the smaller positive solution (> RAY_DISTANCE_MIN) out of the two possible solutions, if one exists.
    // Otherwise return -1.
    double sqrtDiscriminant = sqrt(discriminant);

    double a2 = 2.0 * a;
    double distLarger = (-b + sqrtDiscriminant) / a2;
    if (distLarger < RAY_DISTANCE_MIN) {
        // Both solutions are negative, ray goes in the opposite direction and doesn't hit the sphere.
        return -1;
    }
    double distSmaller = (-b - sqrtDiscriminant) / a2;
    double dist = distSmaller < RAY_DISTANCE_MIN ? distLarger : distSmaller;

    return dist;
}
