#ifndef __LIGHT_H__
#define __LIGHT_H__

typedef struct MaterialDataLight_s          MaterialDataLight;


#include "../types.h"


struct MaterialDataLight_s {
    // Here `color` must be: <light_color> * <luminosity>.
    // Where <light_color> is the standard 24bit Color type.
    // I'm not sure how to define the units for <luminosity> though. You can imagine that when <luminosity> = 1, then this is the same
    // as a matte object. Experiment with this to get the right value.
    Color color;
};

#endif // __LIGHT_H__
