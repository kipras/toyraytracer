#ifndef __MATTE_H__
#define __MATTE_H__

typedef enum   MatteDiffuseAlgo_e           MatteDiffuseAlgo;


enum MatteDiffuseAlgo_e {
    MDA_randomVectorInUnitSphere = 1,
    MDA_randomUnitVectorInUnitSphere,
    MDA_randomVectorInHemisphere,
};

#define MATTE_DIFFUSE_ALGO      MDA_randomUnitVectorInUnitSphere

#endif // __MATTE_H__
