#ifndef __MATERIAL_H__
#define __MATERIAL_H__


typedef struct Material_s       Material;
typedef enum   MaterialType_e   MaterialType;


#include "ray.h"


enum MaterialType_e {
    MT_Matte = 1,      // Diffuse ? Lambert ?
    MT_Metal,          // ?? (Specular ??)
    MT_Glass,          // Refractive ? Dielectric ?
};


struct Material_s {
    MaterialType type;
    void (*scatter)(Ray *, Ray *);
};


extern Material matte;

#endif // __MATERIAL_H__
