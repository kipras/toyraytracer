#include "main.h"

#include "renderer.h"


static inline Color render_background_pixel(App *app, Ray *ray);
static inline Color gradient(Color *colorFrom, Color *colorTo, double valFrom, double valTo, double val);

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

            Color color;
            if (! ray_trace(&app->scene, &ray, &color)) {
                // Ray didn't hit anything - rendering background instead.
                color = render_background_pixel(app, &ray);
            }
            img[imgYArrOffset + imgX] = color;
        }
    }
}

static Color skyTopColor = (Color)COLOR_SKY_TOP;
static Color skyBottomColor = (Color)COLOR_SKY_BOTTOM;

static inline Color render_background_pixel(App *app, Ray *ray)
{
    if (ray->direction.y > app->camera.origin.y) {
        // Draw a sky pixel.
        return gradient(&skyBottomColor, &skyTopColor, 0, 1, ray->direction.y);
    } else {
        // Draw a ground pixel.
        return (Color)COLOR_GROUND;
    }

    // // Draw a two-direction gradient background image.
    // Color color = {
    //     .red = (ray.direction.x + 1) * 128,
    //     .green = (ray.direction.y + 1) * 128,
    //     .blue = 0,
    // };
    // SDL_SetRenderDrawColor(app->renderer, color.red, color.green, color.blue, 255);
    // SDL_RenderDrawPoint(app->renderer, screenX, screenY);
}

static inline Color gradient(Color *colorFrom, Color *colorTo, double valFrom, double valTo, double val)
{
    int16_t redColorRange = colorTo->red - colorFrom->red;
    int16_t greenColorRange = colorTo->green - colorFrom->green;
    int16_t blueColorRange = colorTo->blue - colorFrom->blue;

    double valRange = valTo - valFrom;
    double valWithinRange = val - valFrom;
    double rangePortion = valWithinRange / valRange;

    return (Color){
        .red = colorFrom->red + (redColorRange * rangePortion),
        .green = colorFrom->green + (greenColorRange * rangePortion),
        .blue = colorFrom->blue + (blueColorRange * rangePortion),
    };
}

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
            uint16_t red = 0, green = 0, blue = 0;
            for (uint32_t srcY = srcYBase; srcY < srcYBase+ANTIALIAS_FACTOR; srcY++) {
                uint32_t srcYArrOffset = srcY*srcWidth;
                for (uint32_t srcX = srcXBase; srcX < srcXBase+ANTIALIAS_FACTOR; srcX++) {
                    Color *srcColor = &srcImg[srcYArrOffset + srcX];
                    red     += srcColor->red;
                    green   += srcColor->green;
                    blue    += srcColor->blue;
                }
            }

            // Calculate the resulting pixel color (averaged from the source pixel colors).
            if (ANTIALIAS_FACTOR == 2) {
                // If antialiasing factor is a power of 2 - then we can use the faster shift-right instead of division.
                dstImg[dstY*dstWidth + dstX] = (Color){.red = red >> 2, .green = green >> 2, .blue = blue >> 2};
            } else {
                dstImg[dstY*dstWidth + dstX] = (Color){.red = red/samplesPerPixel, .green = green/samplesPerPixel, .blue = blue/samplesPerPixel};
            }

            srcXBase += ANTIALIAS_FACTOR;
        }
        srcYBase += ANTIALIAS_FACTOR;
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
