#include <stdint.h>
#include <stdio.h>

#include "main.h"

#include "camera.h"
#include "ray.h"
#include "rtalloc.h"
#include "vector.h"


/**
 * v6
 * Attempting to re-start (almost) from scratch - using the "ray tracing in a weekend" camera algorithm.
 * I'll try to render simple stuff again, and test with some layouts of the world where we know what the outcome image is supposed to look
 * like (i.e. debug using that). Start with the simplest possible camera logic and then step by step add test cases and features (as/if
 * needed). Features as in camera looking up/down for example (we'll start without this).
 *
 * Result: the spheres are skewed towards the sides of the image - they are "eggs" (i.e. reverse of being "blunt"). They are skewed towards
 * each side (top/left/right/bottom). This is visible with FOV 90. With FOV 40 - it is barely noticeable.
 */
void cam_set(Camera *cam, Ray *centerRay)
{
    cam->camCenterRay = *centerRay;

    Vector3 *dir = &centerRay->direction;
    double dirVectorLen = vector3_length(dir);      // This should be 1.0.

    // Calculate the width and height of the viewing plane, relative to the length of the viewing direction vector.
    // The viewing direction vector (the vector to the viewing plane's center) is a unit vector (i.e. of length 1).
    // So the viewing plane's width and height are multiples of viewing direction vector's length.
    double fovHorizDeg = FOV_HORIZONTAL;
    double fovHorizRad = ((double)fovHorizDeg / 180) * M_PI;
    double planeWidth  = tan(fovHorizRad/2) * dirVectorLen * 2;
    double planeHeight = planeWidth * ((double)WINDOW_HEIGHT / WINDOW_WIDTH);

    Vector3 dirReverse = *dir;
    vector3_to_unit(&dirReverse);
    vector3_multiply_length(&dirReverse, -1.0);

    Vector3 up = {.x = 0, .y = 0, .z = 1};

    // Calculate a horizontal vector (left to right) of the viewing plane.
    Vector3 *viewPlaneHorizLeftToRight = &cam->viewPlaneHorizLeftToRight;
    vector3_cross(&up, &dirReverse, viewPlaneHorizLeftToRight);
    vector3_to_unit(viewPlaneHorizLeftToRight);
    vector3_multiply_length(viewPlaneHorizLeftToRight, planeWidth);

    Vector3 *viewPlaneHorizRightToLeftHalf = &cam->viewPlaneHorizRightToLeftHalf;
    *viewPlaneHorizRightToLeftHalf = *viewPlaneHorizLeftToRight;
    vector3_multiply_length(viewPlaneHorizRightToLeftHalf, -0.5);

    // Calculate a vertical vector (bottom to up) of the viewing plane.
    Vector3 *viewPlaneVertUpwards = &cam->viewPlaneVertUpwards;
    vector3_cross(&dirReverse, viewPlaneHorizLeftToRight, viewPlaneVertUpwards);
    vector3_to_unit(viewPlaneVertUpwards);
    vector3_multiply_length(viewPlaneVertUpwards, planeHeight);

    Vector3 *viewPlaneVertDownwardsHalf = &cam->viewPlaneVertDownwardsHalf;
    *viewPlaneVertDownwardsHalf = *viewPlaneVertUpwards;
    vector3_multiply_length(viewPlaneVertDownwardsHalf, -0.5);

    // `dir` is a direction vector to view plane's center, and `dirToViewPlaneBottomLeft` is a direction vector to
    // view plane's bottom left corner.
    Vector3 *dirToViewPlaneBottomLeft = &cam->dirToViewPlaneBottomLeft;
    *dirToViewPlaneBottomLeft = *dir;
    vector3_add_to(dirToViewPlaneBottomLeft, viewPlaneVertDownwardsHalf, dirToViewPlaneBottomLeft);
    vector3_add_to(dirToViewPlaneBottomLeft, viewPlaneHorizRightToLeftHalf, dirToViewPlaneBottomLeft);
}

void cam_frame_init(App *app, CameraFrameContext *cfc, uint32_t imgHeight, uint32_t imgWidth)
{
    double verRandForPixel = random_double_0_1_exc() / imgHeight;
    double horRandForPixel = random_double_0_1_exc() / imgWidth;

    cfc->viewPlaneVertUpwardsPartArr        = rtalloc(sizeof(Vector3) * imgHeight);
    cfc->viewPlaneHorizLeftToRightPartArr   = rtalloc(sizeof(Vector3) * imgWidth);

    Camera *cam = &app->camera;
    Vector3 *dirToViewPlaneBottomLeft = &cam->dirToViewPlaneBottomLeft;

    // Generate `Ray.direction` vectors for the left-most pixel of each row of pixels in a rendered image.
    Vector3 *viewPlaneVertUpwards = &cam->viewPlaneVertUpwards;
    for (uint16_t v = 0; v < imgHeight; v++) {
        Vector3 viewPlaneVertUpwardsPart = *viewPlaneVertUpwards;
        vector3_multiply_length(&viewPlaneVertUpwardsPart, ((double)v / imgHeight) + verRandForPixel);

        // The vertical vector also includes the `dirToViewPlaneBottomLeft` part. I.e. this contains the entire `ray.direction` vector for
        // this rendered image row.
        vector3_add_to(dirToViewPlaneBottomLeft, &viewPlaneVertUpwardsPart, &viewPlaneVertUpwardsPart);

        cfc->viewPlaneVertUpwardsPartArr[v] = viewPlaneVertUpwardsPart;
    }

    // Generate `Ray.direction` offset vector for each pixel in a single pixels row (from left to right) in a rendered image.
    Vector3 *viewPlaneHorizLeftToRight = &cam->viewPlaneHorizLeftToRight;
    for (uint16_t u = 0; u < imgWidth; u++) {
        Vector3 viewPlaneHorizLeftToRightPart = *viewPlaneHorizLeftToRight;
        vector3_multiply_length(&viewPlaneHorizLeftToRightPart, ((double)u / imgWidth) + horRandForPixel);
        cfc->viewPlaneHorizLeftToRightPartArr[u] = viewPlaneHorizLeftToRightPart;
    }
}
