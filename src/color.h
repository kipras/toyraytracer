#ifndef __COLOR_H__
#define __COLOR_H__

#include <stdint.h>

#include "rtmath.h"


typedef struct Color_s          Color;
typedef struct Color32_s        Color32;
typedef struct ColorSum_s       ColorSum;


struct Color_s {
    uint8_t     red;
    uint8_t     green;
    uint8_t     blue;
};

struct Color32_s {
    uint32_t    red;
    uint32_t    green;
    uint32_t    blue;
};

#define COLOR_BLACK {  0,   0,   0}
#define COLOR_WHITE {255, 255, 255}
#define COLOR_RED   {255,   0,   0}
#define COLOR_GREEN {  0, 255,   0}
#define COLOR_BLUE  {  0,   0, 255}

#define COLOR_GROUND    {150, 193,  88}     // We tried to go for the color of grass(-ish) here.
#define COLOR_SKY       {162, 204, 228}

#define COLOR_GRADIENT_SKY_TOP      {128, 179, 255}
#define COLOR_GRADIENT_SKY_BOTTOM   {255, 255, 255}

// The ambient light that the sky produces.
#define COLOR_AMBIENT_LIGHT {127, 127, 127}

// The Color32 of a light.
// IMPORTANT: this must only be assigned to the .matData->color of light spheres (i.e. that use matLight material).
#define COLOR32_LIGHT       {1000, 1000, 1000}

// A structure used to blend multiple colors or images (for producing an average).
// We use it to produce the resulting image by averaging multiple raytraced images.
struct ColorSum_s {
    uint32_t    red;
    uint32_t    green;
    uint32_t    blue;
};


/**
 * Converts the source Color32 to a standard 24bit Color.
 * The input Color32 is expected to have all of its values (red/green/blue) to be within the [0, 255] range.
 * Otherwise, the values that exceed 255 will be capped at 255.
 */
static inline void color32_to_color(Color32 *srcColor32, Color *destColor);
static inline Color32 gradient(Color *colorFrom, Color *colorTo, double valFrom, double valTo, double val);


static inline void color32_to_color(Color32 *srcColor32, Color *destColor)
{
    destColor->red = min((uint32_t)255, srcColor32->red);
    destColor->green = min((uint32_t)255, srcColor32->green);
    destColor->blue = min((uint32_t)255, srcColor32->blue);
}

static inline Color32 gradient(Color *colorFrom, Color *colorTo, double valFrom, double valTo, double val)
{
    int16_t redColorRange = colorTo->red - colorFrom->red;
    int16_t greenColorRange = colorTo->green - colorFrom->green;
    int16_t blueColorRange = colorTo->blue - colorFrom->blue;

    double valRange = valTo - valFrom;
    double valWithinRange = val - valFrom;
    double rangePortion = valWithinRange / valRange;

    return (Color32){
        .red = colorFrom->red + (redColorRange * rangePortion),
        .green = colorFrom->green + (greenColorRange * rangePortion),
        .blue = colorFrom->blue + (blueColorRange * rangePortion),
    };
}

#endif // __COLOR_H__
