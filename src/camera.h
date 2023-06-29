#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <stdint.h>


// We define FOV_HORIZONTAL and calculate FOV_VERTICAL from it. We do it this way, assuming that the screen resolution will be either
// square (width == height) or widescreen (width > height). So we define FOV for the one that can be possibly larger (width), just because
// otherwise (theoretically) if you set FOV to the full 360 degrees for the smaller dimension - then you can't extrapolate the FOV of the
// larger dimension from that, because it doesn't make sense for it to be > 360 degrees.
// But i do mean _theoretically_, because a 360 degree FOV doesn't make sense either :)
#define FOV_HORIZONTAL      40

extern double fovHorizDeg;
extern double fovVertDeg;

extern double fovHorizRad;
extern double fovVertRad;


typedef struct Camera_s         Camera;


#include "ray.h"


struct Camera_s {
    // The initial center ray that was given, from which the Camera was initialized.
    // This is stored for debugging purposes only (this value is not used by raytracing, after initial camera initialization).
    Ray camCenterRay;

    uint16_t imgHeight;
    uint16_t imgWidth;

    // An imgHeight*imgWidth array of pre-computed camera rays for rendering each pixel of img, from bottom left camera corner to top right.
    Ray *camRays;
};


/**
 * Initializes FOV variables and `cam->camRays` (based on FOV and the given `centerRay`, which is a ray that is pointing in the direction
 * that should be displayed at the center of the rendered image).
 * IMPORTANT: this requires for `centerRay->direction` to be a unit vector.
 */
void cam_set(Camera *cam, Ray *centerRay, uint16_t imgHeight, uint16_t imgWidth);

#endif // __CAMERA_H__
