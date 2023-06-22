#include "color.h"
#include "renderer.h"


// static inline Color render_background_pixel(App *app, Ray *ray);

/**
 * Anti-aliases (larger) `srcImg` into `dstImg`, by averaging ANTIALIAS_FACTOR^2 pixels into 1 pixel (using grid algorithm).
 */
static void image_antialias(Color32 *srcImg, Color32 *dstImg, uint32_t dstHeight, uint32_t dstWidth);


void render_frame_img_antialiased(App *app, Color32 *img, uint32_t imgHeight, uint32_t imgWidth)
{
    uint32_t upsampledImgHeight = imgHeight * ANTIALIAS_FACTOR;
    uint32_t upsampledImgWidth = imgWidth * ANTIALIAS_FACTOR;
    Color32 upsampledImage[upsampledImgHeight * upsampledImgWidth];

    // Produce the (larger) upsampled image.
    render_frame_img(app, upsampledImage, upsampledImgHeight, upsampledImgWidth);

    // Produce the final image, by anti-aliasing the (larger) upsampled image.
    image_antialias(upsampledImage, img, imgHeight, imgWidth);
}

void render_frame_img(App *app, Color32 *img, uint32_t imgHeight, uint32_t imgWidth)
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

            Color32 color32;
            if (! ray_trace(&rtContext, &app->scene, &unitRay, &color32)) {
                color32 = (Color32)COLOR_BLACK;

                // // Ray didn't hit anything - rendering background instead.
                // color = render_background_pixel(app, &ray);
            }

            img[imgYArrOffset + imgX] = color32;
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

static void image_antialias(Color32 *srcImg, Color32 *dstImg, uint32_t dstHeight, uint32_t dstWidth)
{
    uint32_t srcWidth  = dstWidth*ANTIALIAS_FACTOR;
    uint16_t samplesPerPixel = ANTIALIAS_FACTOR*ANTIALIAS_FACTOR;

    uint32_t srcYBase, srcXBase;
    srcYBase = 0;
    for (uint32_t dstY = 0; dstY < dstHeight; dstY++) {
        srcXBase = 0;
        for (uint32_t dstX = 0; dstX < dstWidth; dstX++) {
            // Calculate the antialiased pixel in dstImg from multiple pixels in srcImg.
            uint64_t red = 0, green = 0, blue = 0;
            for (uint32_t srcY = srcYBase; srcY < srcYBase+ANTIALIAS_FACTOR; srcY++) {
                uint32_t srcYArrOffset = srcY*srcWidth;
                for (uint32_t srcX = srcXBase; srcX < srcXBase+ANTIALIAS_FACTOR; srcX++) {
                    // Cap at 32 bits*ANTIALIAS_FACTOR (we will divide by ANTIALIAS_FACTOR later)
                    uint64_t cap = 0xFFFFFFFE * ANTIALIAS_FACTOR;

                    Color32 *srcColor = &srcImg[srcYArrOffset + srcX];
                    red     = min(cap, red + srcColor->red);
                    green   = min(cap, green + srcColor->green);
                    blue    = min(cap, blue + srcColor->blue);
                }
            }

            // Calculate the resulting pixel color (averaged from the source pixel colors).
            if (ANTIALIAS_FACTOR == 2) {
                // If antialiasing factor is a power of 2 - then we can use the faster shift-right instead of division.
                dstImg[dstY*dstWidth + dstX] = (Color32){.red = red >> 2, .green = green >> 2, .blue = blue >> 2};
            } else {
                dstImg[dstY*dstWidth + dstX] = (Color32){.red = red/samplesPerPixel, .green = green/samplesPerPixel, .blue = blue/samplesPerPixel};
            }

            srcXBase += ANTIALIAS_FACTOR;
        }
        srcYBase += ANTIALIAS_FACTOR;
    }
}

void blend_frame(Color32 *summedFrames, uint32_t frameNum, Color32 *frameImg, Color *resImg, uint32_t imgHeight, uint32_t imgWidth)
{
    for (uint32_t y = 0; y < imgHeight; y++) {
        uint32_t yArrOffset = y * imgWidth;
        for (uint32_t x = 0; x < imgWidth; x++) {
            Color32 *summedPixel = &summedFrames[yArrOffset + x];
            Color32 *frameImgPixel = &frameImg[yArrOffset + x];

            // Using 64 bit int to store the sum, to avoid overflow (if the sum overflows).
            uint64_t sumRed   = summedPixel->red + frameImgPixel->red,
                     sumGreen = summedPixel->green + frameImgPixel->green,
                     sumBlue  = summedPixel->blue + frameImgPixel->blue;

            // Store sums, capped at 32 bits.
            summedPixel->red   = min(0xFFFFFFFE, sumRed);
            summedPixel->green = min(0xFFFFFFFE, sumGreen);
            summedPixel->blue  = min(0xFFFFFFFE, sumBlue);

            // Store resulting image, after capping each color at 8 bits (they **must** be capped).
            Color *resImgPixel = &resImg[yArrOffset + x];
            resImgPixel->red   = min(0xFE, round((double)summedPixel->red / frameNum));
            resImgPixel->green = min(0xFE, round((double)summedPixel->green / frameNum));
            resImgPixel->blue  = min(0xFE, round((double)summedPixel->blue / frameNum));
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
            SDL_SetRenderDrawColor(app->renderer, color->red, color->green, color->blue, 255);
            SDL_RenderDrawPoint(app->renderer, imgX, imgY);
        }
        imgYArrOffset += imgWidth;
    }

    // Draw the SDL buffer to the screen.
    SDL_RenderPresent(app->renderer);
}
