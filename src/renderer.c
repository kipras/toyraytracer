#include <stdio.h>

#include "color.h"
#include "ray_inline_fns.h"
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
    CameraFrameContext cfc;
    cam_frame_init(app, &cfc, imgHeight, imgWidth);

    Ray ray = {
        .origin = app->camera.camCenterRay.origin,
        .direction = {.x = 0, .y = 0, .z = 0},
    };

    for (uint32_t row = 0; row < imgHeight; row++) {
        uint32_t imgV = imgHeight - row - 1;
        uint32_t imgArrRowOffset = row * imgWidth;

        for (uint32_t imgU = 0; imgU < imgWidth; imgU++) {
            // The produced `ray.direction` vector is a unit vector. This is needed for dot product later on, by some materials.
            // That way those materials don't need to compute the unit vector themselves.
            cam_frame_get_ray_direction(&cfc, imgU, imgV, &ray.direction);

            RTContext rtContext;
            ray_trace_context_init(&rtContext);

            Color color;
            if (! ray_trace(&rtContext, &app->scene, &ray, &color)) {
                color = (Color)COLOR_BLACK;

                // // Ray didn't hit anything - rendering background instead.
                // color = render_background_pixel(app, &ray);
            }

            img[imgArrRowOffset + imgU] = color;
        }
    }
}

// static inline Color render_background_pixel(App *app, Ray *ray)
// {
//     if (ray->direction.y > app->cameraCenterRay.origin.y) {
//         // Draw a sky pixel.
//         return gradient(&skyBottomColor, &skyTopColor, 0, 1, ray->direction.y);
//     } else {
//         // Draw a ground pixel.
//         return (Color)COLOR_WHITE;
//         // return (Color)COLOR_GROUND;
//     }

//     // if (ray->direction.y > app->cameraCenterRay.origin.y) {
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
//     // SDL_SetRenderDrawColor(app->sdlRenderer, color.red, color.green, color.blue, 255);
//     // SDL_RenderDrawPoint(app->sdlRenderer, screenX, screenY);
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

void renderer_init(App *app)
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(app->windowWidth, app->windowHeight, 0, &app->sdlWindow, &app->sdlRenderer);
    // SDL_RenderSetScale(renderer, 4, 4);

    // SDL_SetRenderDrawColor(app->sdlRenderer, 0, 0, 0, 255);
    // SDL_RenderClear(app->sdlRenderer);
    // SDL_RenderPresent(app->sdlRenderer);

    // Initialize the SDL texture to which we will draw everything.
    app->sdlTexture = SDL_CreateTexture(
        app->sdlRenderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, app->windowWidth, app->windowHeight);

    if (app->sdlTexture == NULL) {
        const char *err = SDL_GetError();
        log_err("Fatal SDL_CreateTexture() error: %s", err);
        exit(1);
    }

    int status = SDL_SetTextureBlendMode(app->sdlTexture, SDL_BLENDMODE_NONE);
    if (status != 0) {
        const char *err = SDL_GetError();
        log_err("Fatal SDL_SetTextureBlendMode() error: %s", err);
        exit(1);
    }
}

void draw_img_to_screen(App *app, Color *img, uint32_t imgHeight, uint32_t imgWidth)
{
    // // Clear the existing SDL image buffer (it becomes all black).
    // SDL_SetRenderDrawColor(app->sdlRenderer, 0, 0, 0, 255);
    // SDL_RenderClear(app->sdlRenderer);

    int status;
    uint8_t *pixels;
    int pitch;
    status = SDL_LockTexture(app->sdlTexture, NULL, (void **)&pixels, &pitch);
    if (status != 0) {
        const char *err = SDL_GetError();
        log_err("Fatal SDL_LockTexture() error: %s", err);
        exit(1);
    }

    // Draw the image to a pixel array.
    // If we are using a locked streaming texture - then we are drawing directly into the SDL texture's pixel array.
    // Otherwise (we are using a static texture) - we will later copy this pixel array into the SDL texture using
    // SDL_UpdateTexture().
    uint32_t imgVArrOffset = 0;
    for (uint32_t imgV = 0; imgV < imgHeight; imgV++) {
        for (uint32_t imgU = 0; imgU < imgWidth; imgU++) {
            Color *color = &img[imgVArrOffset + imgU];

            // Convert colors expressed as floating point (in the range [0, 1]) into 8 bit integers.
            // And cap them, because inputs may actually be > 1.0.
            uint8_t red   = min(255, round(min(1.0, color->red)   * 256)),
                    green = min(255, round(min(1.0, color->green) * 256)),
                    blue  = min(255, round(min(1.0, color->blue)  * 256));

            uint32_t pixelsArrOffset = (imgVArrOffset + imgU) * 3;
            pixels[pixelsArrOffset] = red;
            pixels[pixelsArrOffset + 1] = green;
            pixels[pixelsArrOffset + 2] = blue;
        }
        imgVArrOffset += imgWidth;
    }

    SDL_UnlockTexture(app->sdlTexture);

    status = SDL_RenderClear(app->sdlRenderer);
    if (status != 0) {
        const char *err = SDL_GetError();
        log_err("Fatal SDL_RenderClear() error: %s", err);
        exit(1);
    }

    status = SDL_RenderCopy(app->sdlRenderer, app->sdlTexture, NULL, NULL);
    if (status != 0) {
        const char *err = SDL_GetError();
        log_err("Fatal SDL_RenderCopy() error: %s", err);
        exit(1);
    }

    SDL_RenderPresent(app->sdlRenderer);
}
