#include <stdint.h>
#include <stdio.h>

#include "main.h"

#include "camera.h"
#include "ray.h"
#include "rtalloc.h"
#include "vector.h"


double fovHorizDeg;
double fovVertDeg;

double fovHorizRad;
double fovVertRad;


/**
 * v6
 * Attempting to re-start (almost) from scratch - using the "ray tracing in a weekend" camera algorithm.
 * I'll try to render simple stuff again, and test with some layouts of the world where we know what the outcome image is supposed to look
 * like (i.e. debug using that). Start with the simplest possible camera logic and then step by step add test cases and features (as/if
 * needed). Features as in camera looking up/down for example (we'll start without this).
 *
 * Result: the spheres are skewed towards the sides of the image - they are "eggs" (i.e. reverse of being "blunt"). They are skewed towards
 * each side (top/left/right/bottom).
 */

void cam_set(Camera *cam, Ray *centerRay, uint16_t imgHeight, uint16_t imgWidth)
{
    fovHorizDeg = FOV_HORIZONTAL;
    fovVertDeg = ((double)(WINDOW_HEIGHT * fovHorizDeg)) / WINDOW_WIDTH;
    fovHorizRad = ((double)fovHorizDeg / 180) * M_PI;
    fovVertRad = ((double)fovVertDeg / 180) * M_PI;

    cam->camCenterRay = *centerRay;

    // Allocate the cam->camRays array for pre-computed rays.
    Ray *camRays = rtalloc(sizeof(Ray) * (imgHeight*imgWidth));
    cam->camRays = camRays;

    Vector3 *org = &centerRay->origin;
    Vector3 *dir = &centerRay->direction;
    double dirVectorLen = vector3_length(dir);      // This should be 1.0.

    // Calculate the width and height of the viewing plane, relative to the length of the viewing direction vector.
    // The viewing direction vector (the vector to the viewing plane's center) is a unit vector (i.e. of length 1).
    // So the viewing plane's width and height are multiples of viewing direction vector's length.
    double planeWidth  = tan(fovHorizRad/2) * dirVectorLen * 2;
    double planeHeight = tan(fovVertRad/2) * dirVectorLen * 2;

    Vector3 dirReverse = *dir;
    vector3_to_unit(&dirReverse);
    vector3_multiply_length(&dirReverse, -1.0);

    Vector3 up = {.x = 0, .y = 0, .z = 1};

    // Calculate a horizontal vector (left to right) of the viewing plane.
    Vector3 viewPlaneHorizLeftToRight;
    vector3_cross(&up, &dirReverse, &viewPlaneHorizLeftToRight);
    vector3_to_unit(&viewPlaneHorizLeftToRight);
    vector3_multiply_length(&viewPlaneHorizLeftToRight, planeWidth);

    Vector3 viewPlaneHorizRightToLeftHalf = viewPlaneHorizLeftToRight;
    vector3_multiply_length(&viewPlaneHorizRightToLeftHalf, -0.5);

    // Calculate a vertical vector (bottom to up) of the viewing plane.
    Vector3 viewPlaneVertUpwards;
    vector3_cross(&dirReverse, &viewPlaneHorizLeftToRight, &viewPlaneVertUpwards);
    vector3_to_unit(&viewPlaneVertUpwards);
    vector3_multiply_length(&viewPlaneVertUpwards, planeHeight);

    Vector3 viewPlaneVertDownwardsHalf = viewPlaneVertUpwards;
    vector3_multiply_length(&viewPlaneVertDownwardsHalf, -0.5);

    // `dir` is a direction vector to view plane's center, and `dirToViewPlaneBottomLeft` is a direction vector to
    // view plane's bottom left corner.
    Vector3 dirToViewPlaneBottomLeft = *dir;
    vector3_add_to(&dirToViewPlaneBottomLeft, &viewPlaneVertDownwardsHalf, &dirToViewPlaneBottomLeft);
    vector3_add_to(&dirToViewPlaneBottomLeft, &viewPlaneHorizRightToLeftHalf, &dirToViewPlaneBottomLeft);

    for (uint16_t v = 0; v < imgHeight; v++) {
        Vector3 viewPlaneVertUpwardsPart = viewPlaneVertUpwards;
        vector3_multiply_length(&viewPlaneVertUpwardsPart, (double)v / imgHeight);

        Vector3 rowLeftRayDir;
        vector3_add_to(&dirToViewPlaneBottomLeft, &viewPlaneVertUpwardsPart, &rowLeftRayDir);

        uint32_t arrVOffset = v * imgWidth;
        for (uint16_t u = 0; u < imgWidth; u++) {
            Vector3 viewPlaneHorizLeftToRightPart = viewPlaneHorizLeftToRight;
            vector3_multiply_length(&viewPlaneHorizLeftToRightPart, (double)u / imgWidth);

            Ray *camRay = &cam->camRays[arrVOffset + u];
            camRay->origin = *org;

            Vector3 *camRayDir = &camRay->direction;
            vector3_add_to(&rowLeftRayDir, &viewPlaneHorizLeftToRightPart, camRayDir);
            vector3_to_unit(camRayDir);
        }
    }
}
