#include <stdio.h>
#include <float.h>

#include "ray.h"
#include "vector.h"


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

// bool ray_hits_sphere(Ray *ray, Sphere *sphere, Vector3 *hitPoint)
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
    double c = vector3_dot(&ray_orig_and_sphere_diff, &ray_orig_and_sphere_diff) - (sphere->radius*sphere->radius);

    double discriminant = (b*b) - (4*a*c);

    if (discriminant < 0) {
        return -1;
    }

    // Ray hits the sphere. We want to return the closer (out of the two possible points) where ray hits the sphere.
    // To do that we use - out of the two (+-) solutions (to get the smaller value).
    //
    // However, there is a nuance, regarding the sky sphere - when hitting the sky sphere, the rays will always hit from the _inside_ of
    // the sky sphere. In that case: sqrt(discriminant) > -b. So  the smaller value is negative, but the larger value is positive.
    // So we do an additional check for this, and return the larger value in this case.
    double sqrtDiscriminant = sqrt(discriminant);
    double signedSqrtDiscriminant;
    if (sqrtDiscriminant > -b) {
        signedSqrtDiscriminant = sqrtDiscriminant;
    } else {
        signedSqrtDiscriminant = -sqrtDiscriminant;
    }

    double dist = (-b + signedSqrtDiscriminant) / (2*a);

    return dist;

    // // Set `hitPoint` to the point on the `sphere` where `ray` hits.
    // ray_point(ray, dist, hitPoint);

    // return true;
}
