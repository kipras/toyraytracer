#ifndef __METAL_H__
#define __METAL_H__

typedef struct MaterialDataMetal_s          MaterialDataMetal;


struct MaterialDataMetal_s {
    // The fuzziness of the reflected rays. Should be set to >= 0.
    // If set to 0 (no fuzziness) - then the material is perfect glass (i.e. produces perfect reflections).
    // The higher this value - the more the material behaves like a matte material (where reflected rays scatter in any direction) and less
    // like a metal material (where rays reflect with mirror reflections).
    // I.e. it gives a "brushed" metal look (like the one of christmas tree bubbles).
    double fuzziness;
};

#endif // __METAL_H__
