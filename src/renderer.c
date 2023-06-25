#include "color.h"
#include "renderer.h"


// static inline Color render_background_pixel(App *app, Ray *ray);

/**
 * Anti-aliases (larger) `srcImg` into `dstImg`, by averaging ANTIALIAS_FACTOR^2 pixels into 1 pixel (using grid algorithm).
 */
static void image_antialias(Color *srcImg, Color *dstImg, uint32_t dstHeight, uint32_t dstWidth);


void render_frame_img_antialiased(App *app, Color *img, uint32_t imgHeight, uint32_t imgWidth)
{
    uint32_t upsampledImgHeight = imgHeight * ANTIALIAS_FACTOR;
    uint32_t upsampledImgWidth = imgWidth * ANTIALIAS_FACTOR;
    Color upsampledImage[upsampledImgHeight * upsampledImgWidth];

    // Produce the (larger) upsampled image.
    render_frame_img(app, upsampledImage, upsampledImgHeight, upsampledImgWidth);

    // Produce the final image, by anti-aliasing the (larger) upsampled image.
    image_antialias(upsampledImage, img, imgHeight, imgWidth);
}

void render_frame_img(App *app, Color *img, uint32_t imgHeight, uint32_t imgWidth)
{
    Ray ray = app->camera;
    Ray unitRay;

    uint32_t imgMidHeight = imgHeight >> 1;
    uint32_t imgMidWidth = imgWidth >> 1;

    for (uint32_t imgY = 0; imgY < imgHeight; imgY++) {
        // Ray direction is from [y=1, x=-1] (top left corner) to [y=-1, x=1] (bottom right corner).
        // Depth (z) remains unchanged at -1.
        ray.direction.y = ((double)(imgHeight - imgY) / imgMidHeight) - 1;
        // printf("\n");
        // printf("ray.direction.y = %f\n", ray.direction.y);
        uint32_t imgYArrOffset = imgY * imgWidth;
        for (uint32_t imgX = 0; imgX < imgWidth; imgX++) {
            ray.direction.x = ((double)imgX / imgMidWidth) - 1;
            // printf("ray.direction.x = %f\n", ray.direction.x);

            RTContext rtContext;
            ray_trace_context_init(&rtContext);

            // We will pass a ray, where `direction` is a unit vector, because this is needed for dot product later on, by some materials.
            // That way those materials don't need to compute the unit vector themselves.
            unitRay = ray;
            vector3_to_unit(&unitRay.direction);

            Color color;
            if (! ray_trace(&rtContext, &app->scene, &unitRay, &color)) {
                color = (Color)COLOR_BLACK;

                // // Ray didn't hit anything - rendering background instead.
                // color = render_background_pixel(app, &ray);
            }

            img[imgYArrOffset + imgX] = color;
        }
    }
}

// static inline Color render_background_pixel(App *app, Ray *ray)
// {
//     if (ray->direction.y > app->camera.origin.y) {
//         // Draw a sky pixel.
//         return gradient(&skyBottomColor, &skyTopColor, 0, 1, ray->direction.y);
//     } else {
//         // Draw a ground pixel.
//         return (Color)COLOR_WHITE;
//         // return (Color)COLOR_GROUND;
//     }

//     // if (ray->direction.y > app->camera.origin.y) {
//     //     // Draw a sky pixel.
//     //     return gradient(&skyBottomColor, &skyTopColor, 0, 1, ray->direction.y);
//     // } else {
//     //     // Draw a ground pixel.
//     //     return (Color)COLOR_BLACK;
//     //     // return (Color)COLOR_GROUND;
//     // }

//     // // Draw a two-direction gradient background image.
//     // Color color = {
//     //     .red = (ray.direction.x + 1) * 128,
//     //     .green = (ray.direction.y + 1) * 128,
//     //     .blue = 0,
//     // };
//     // SDL_SetRenderDrawColor(app->renderer, color.red, color.green, color.blue, 255);
//     // SDL_RenderDrawPoint(app->renderer, screenX, screenY);
// }

static void image_antialias(Color *srcImg, Color *dstImg, uint32_t dstHeight, uint32_t dstWidth)
{
    uint32_t srcWidth  = dstWidth*ANTIALIAS_FACTOR;
    uint16_t samplesPerPixel = ANTIALIAS_FACTOR*ANTIALIAS_FACTOR;

    uint32_t srcYBase, srcXBase;
    srcYBase = 0;
    for (uint32_t dstY = 0; dstY < dstHeight; dstY++) {
        srcXBase = 0;
        for (uint32_t dstX = 0; dstX < dstWidth; dstX++) {
            // Calculate the antialiased pixel in dstImg from multiple pixels in srcImg.
            double red = 0, green = 0, blue = 0;
            for (uint32_t srcY = srcYBase; srcY < srcYBase+ANTIALIAS_FACTOR; srcY++) {
                uint32_t srcYArrOffset = srcY*srcWidth;
                for (uint32_t srcX = srcXBase; srcX < srcXBase+ANTIALIAS_FACTOR; srcX++) {
                    Color *srcColor = &srcImg[srcYArrOffset + srcX];
                    red     += srcColor->red;
                    green   += srcColor->green;
                    blue    += srcColor->blue;
                }
            }

            dstImg[dstY*dstWidth + dstX] = (Color){.red = red/samplesPerPixel, .green = green/samplesPerPixel, .blue = blue/samplesPerPixel};

            srcXBase += ANTIALIAS_FACTOR;
        }
        srcYBase += ANTIALIAS_FACTOR;
    }
}

void blend_frame(Color *summedFrames, uint32_t frameNum, Color *frameImg, Color *resImg, uint32_t imgHeight, uint32_t imgWidth)
{
    for (uint32_t y = 0; y < imgHeight; y++) {
        uint32_t yArrOffset = y * imgWidth;
        for (uint32_t x = 0; x < imgWidth; x++) {
            Color *summedPixel = &summedFrames[yArrOffset + x];
            Color *frameImgPixel = &frameImg[yArrOffset + x];

            // Update sums.
            summedPixel->red   += frameImgPixel->red;
            summedPixel->green += frameImgPixel->green;
            summedPixel->blue  += frameImgPixel->blue;

            // Store resulting image. Note that pixel color component values here may exceed 1.0.
            // They will be capped later, when drawing to the screen.
            Color *resImgPixel = &resImg[yArrOffset + x];
            resImgPixel->red   = summedPixel->red / frameNum;
            resImgPixel->green = summedPixel->green / frameNum;
            resImgPixel->blue  = summedPixel->blue / frameNum;
        }
    }
}

void draw_img_to_screen(App *app, Color *img, uint32_t imgHeight, uint32_t imgWidth)
{
    // Clear the existing SDL image buffer (it becomes all black).
    SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
    SDL_RenderClear(app->renderer);

    // Draw the image to the SDL buffer.
    uint32_t imgYArrOffset = 0;
    for (uint32_t imgY = 0; imgY < imgHeight; imgY++) {
        for (uint32_t imgX = 0; imgX < imgWidth; imgX++) {
            Color *color = &img[imgYArrOffset + imgX];

            // Convert colors expressed as floating point (in the range [0, 1]) into 8 bit integers.
            // And cap them, because inputs may actually be > 1.0.
            uint8_t red   = min(255, round(min(1.0, color->red)   * 256)),
                    green = min(255, round(min(1.0, color->green) * 256)),
                    blue  = min(255, round(min(1.0, color->blue)  * 256));

            SDL_SetRenderDrawColor(app->renderer, red, green, blue, 255);
            SDL_RenderDrawPoint(app->renderer, imgX, imgY);
        }
        imgYArrOffset += imgWidth;
    }

    // Draw the SDL buffer to the screen.
    SDL_RenderPresent(app->renderer);
}
