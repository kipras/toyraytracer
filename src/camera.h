#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <stdint.h>


// We define FOV_HORIZONTAL and calculate FOV_VERTICAL from it. We do it this way, assuming that the screen resolution will be either
// square (width == height) or widescreen (width > height). So we define FOV for the one that can be possibly larger (width), just because
// otherwise (theoretically) if you set FOV to the full 360 degrees for the smaller dimension - then you can't extrapolate the FOV of the
// larger dimension from that, because it doesn't make sense for it to be > 360 degrees.
// But i do mean _theoretically_, because a 360 degree FOV doesn't make sense either :)
#define FOV_HORIZONTAL      40


typedef struct Camera_s             Camera;
typedef struct CameraFrameContext_s CameraFrameContext;


#include "ray.h"


struct Camera_s {
    // The initial center ray that was given, from which the Camera was initialized.
    // This is stored for debugging purposes only (this value is not used by raytracing, after initial camera initialization).
    Ray camCenterRay;

    uint16_t imgHeight;
    uint16_t imgWidth;

    Vector3 viewPlaneHorizLeftToRight;
    Vector3 viewPlaneHorizRightToLeftHalf;
    Vector3 viewPlaneVertUpwards;
    Vector3 viewPlaneVertDownwardsHalf;

    // `camCenterRay.direction` is a direction vector to view plane's center, and `dirToViewPlaneBottomLeft` is a direction vector to
    // view plane's bottom left corner.
    Vector3 dirToViewPlaneBottomLeft;
};

struct CameraFrameContext_s {
    // Array of precalculated vertical vector parts, for the left-most pixel of each pixels row of the rendered image.
    // NOTE: This contains the entire `ray.direction` vector for this rendered image row, pointing to the left-most (first) pixel of the
    // rendered image (i.e. u=0). This is done for slightly better performance - so that we would not need to add
    // `Camera->dirToViewPlaneBottomLeft` vector when calculating every rendered image pixel `Ray.direction` vector (as it is already
    // pre-added here).
    Vector3 *viewPlaneVertUpwardsPartArr;

    // Array of precalculated horizontal vector parts, for each column of the rendered image.
    // This contains only the offset (part) of the vector to be added to the final `Ray.direction` vector (of each pixel column).
    Vector3 *viewPlaneHorizLeftToRightPartArr;
};


/**
 * Initializes FOV variables and `cam->camRays` (based on FOV and the given `centerRay`, which is a ray that is pointing in the direction
 * that should be displayed at the center of the rendered image).
 * IMPORTANT: this requires for `centerRay->direction` to be a unit vector.
 */
void cam_set(Camera *cam, Ray *centerRay);

/**
 * Initializes a CameraFrameContext for rendering a single frame.
 *
 * This initializes the randomization (within pixel boundaries) of the rendered image pixels camera rays directions.
 * The ray direction is randomized, within a pixel's boundaries. And because when rendering we blend together many renders of the same
 * scene (but each frame has slightly different camera ray directions (for each pixel), because of this randomization):
 * * this reduces the noise effect (where the color of nearby pixels of the same rendered object varies greatly, when it really shouldn't),
 *   because one of the reasons for this effect (especially for materials that don't add randomization for the scattered rays (e.g. glass
 *   or metal with 0 fuzziness (i.e. mirror))) is that the rays for nearby pixels (after all of their bounces) may end up hitting very
 *   different things as their final hit. So, if there is no randomization within the path of a ray - each pixel then gets the same color
 *   for every frame, but nearby pixels can get very different colors.
 *   This added randomization helps with reducing this noise (but doesn't remove it completely).
 * * this gives us cheap and good anti-aliasing.
 * For each frame - different random offsets are generated, so this function is called at the beginning of rendering each frame.
 */
void cam_frame_init(App *app, CameraFrameContext *cfc, uint32_t imgHeight, uint32_t imgWidth);

/**
 * Calculates a camera ray direction for the u (vertical), v (horizontal) coordinates of the image for a frame (with context `cfc`) and
 * stores that ray direction in `rayDir`.
 * The produced `rayDir` vector is a unit vector.
 */
static inline void cam_frame_get_ray_direction(CameraFrameContext *cfc, uint32_t u, uint32_t v, Vector3 *rayDir)
{
    vector3_add_to(&cfc->viewPlaneVertUpwardsPartArr[v], &cfc->viewPlaneHorizLeftToRightPartArr[u], rayDir);
    vector3_to_unit(rayDir);
}

#endif // __CAMERA_H__
