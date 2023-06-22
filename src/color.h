#ifndef __COLOR_H__
#define __COLOR_H__

#include <stdint.h>


typedef struct Color_s          Color;


struct Color_s {
    uint8_t     red;
    uint8_t     green;
    uint8_t     blue;
};
#define COLOR_BLACK {  0,   0,   0}
#define COLOR_WHITE {255, 255, 255}
#define COLOR_RED   {255,   0,   0}
#define COLOR_GREEN {  0, 255,   0}
#define COLOR_BLUE  {  0,   0, 255}

#define COLOR_GROUND    {150, 193,  88}
#define COLOR_SKY       {162, 204, 228}

#define COLOR_SKY_TOP       {128, 179, 255}
#define COLOR_SKY_BOTTOM    {255, 255, 255}


static inline Color gradient(Color *colorFrom, Color *colorTo, double valFrom, double valTo, double val);


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

#endif // __COLOR_H__
