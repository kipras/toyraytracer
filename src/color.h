#ifndef __COLOR_H__
#define __COLOR_H__

#include <stdint.h>

#include "rtmath.h"


typedef struct Color_s          Color;


struct Color_s {
    double  red;
    double  green;
    double  blue;
};

#define COLOR_BLACK { 0, 0, 0}
#define COLOR_WHITE { 1, 1, 1}
#define COLOR_RED   { 1, 0, 0}
#define COLOR_GREEN { 0, 1, 0}
#define COLOR_BLUE  { 0, 0, 1}

#define COLOR_GROUND    {0.586, 0.750, 0.340}     // We tried to go for the color of grass(-ish) here.
#define COLOR_SKY       {0.633, 0.797, 0.890}

#define COLOR_GRADIENT_SKY_TOP      {0.5, 0.7,   1}
#define COLOR_GRADIENT_SKY_BOTTOM   {  1,   1,   1}

// The ambient light that the sky produces.
#define COLOR_AMBIENT_LIGHT {0.5, 0.5, 0.5}

// The color of a white light (x * <white>).
#define COLOR_LIGHT       {40, 40, 40}


static inline Color gradient(Color *colorFrom, Color *colorTo, double valFrom, double valTo, double val);


static inline Color gradient(Color *colorFrom, Color *colorTo, double valFrom, double valTo, double val)
{
    double redColorRange = colorTo->red - colorFrom->red;
    double greenColorRange = colorTo->green - colorFrom->green;
    double blueColorRange = colorTo->blue - colorFrom->blue;

    double valRange = valTo - valFrom;
    double valWithinRange = val - valFrom;
    double rangePortion = valWithinRange / valRange;

    return (Color){
        .red = colorFrom->red + (redColorRange * rangePortion),
        .green = colorFrom->green + (greenColorRange * rangePortion),
        .blue = colorFrom->blue + (blueColorRange * rangePortion),
    };
}

#endif // __COLOR_H__
