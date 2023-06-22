#ifndef __RENDERER_H__
#define __RENDERER_H__

#include "main.h"

#include "color.h"


/**
 * Similar to render_frame_img(), but renders an anti-aliased image, by averaging ANTIALIAS_FACTOR^2 pixels into 1 (with grid algorithm).
 */
void render_frame_img_antialiased(App *app, Color *img, uint32_t imgHeight, uint32_t imgWidth);

/**
 * Renders an image by ray-tracing the scene.
 */
void render_frame_img(App *app, Color *img, uint32_t imgHeight, uint32_t imgWidth);

/**
 * Adds each pixel from the image `frameImg` to `summedFrames` summed image, and produces the averaged `resImg` image, by dividing the
 * pixels in `allFrames` by `frameNum`. `frameNum` must be set by the caller to the amount of total frames rendered, including this frame
 * (i.e. starting from 1).
 */
void blend_frame(ColorSum *summedFrames, uint32_t frameNum, Color *frameImg, Color *resImg, uint32_t imgHeight, uint32_t imgWidth);

/**
 * Draws `img` to the screen.
 */
void draw_img_to_screen(App *app, Color *img, uint32_t imgHeight, uint32_t imgWidth);

#endif // __RENDERER_H__
