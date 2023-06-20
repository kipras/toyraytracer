#ifndef __RENDERER_H__
#define __RENDERER_H__

#include "main.h"


/**
 * Similar to render_frame_img(), but renders an anti-aliased image, by averaging ANTIALIAS_FACTOR^2 pixels into 1 (with grid algorithm).
 */
void render_frame_img_antialiased(App *app, Color *img, uint32_t imgHeight, uint32_t imgWidth);

/**
 * Renders an image by ray-tracing the scene.
 */
void render_frame_img(App *app, Color *img, uint32_t imgHeight, uint32_t imgWidth);

/**
 * Anti-aliases (larger) `srcImg` into `dstImg`, by averaging ANTIALIAS_FACTOR^2 pixels into 1 (with grid algorithm).
 */
void image_antialias(Color *srcImg, Color *dstImg, uint32_t dstHeight, uint32_t dstWidth);

/**
 * Draws `img` to the screen.
 */
void draw_img_to_screen(App *app, Color *img, uint32_t imgHeight, uint32_t imgWidth);

#endif // __RENDERER_H__
