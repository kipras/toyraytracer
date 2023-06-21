#ifndef __RANDOM_H__
#define __RANDOM_H__

#include <stdlib.h>


/**
 * Seeds the random number generator.
 */
static inline void random_seed(unsigned int seed)
{
    srand(seed);
}

/**
 * Generates and returns a random double in the range [0, 1), using C built-in rand().
 */
static inline double random_double_0_1_exc()
{
    return ((double)rand()) / (RAND_MAX + 1.0);
}

/**
 * Generates and returns a random double in the range [0, 1], using C built-in rand().
 */
static inline double random_double_0_1_inc()
{
    return ((double)rand()) / RAND_MAX;
}

/**
 * Generates and returns a random double in the range [min, max), using C built-in rand().
 */
static inline double random_double_exc(double min, double max)
{
    return min + ((max - min) * random_double_0_1_exc());
}

/**
 * Generates and returns a random double in the range [min, max], using C built-in rand().
 */
static inline double random_double_inc(double min, double max)
{
    return min + ((max - min) * random_double_0_1_inc());
}

/**
 * Generates and returns a random int in the range [min, max), using C built-in rand().
 */
static inline int random_int_exc(int min, int max)
{
    return (int)random_double_exc(min, max);
}

/**
 * Generates and returns a random int in the range [min, max], using C built-in rand().
 */
static inline int random_int_inc(int min, int max)
{
    return (int)random_double_inc(min, max);
}

#endif // __RANDOM_H__
