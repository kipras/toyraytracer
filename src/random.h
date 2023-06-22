#ifndef __RANDOM_H__
#define __RANDOM_H__

#include <stdlib.h>


extern __uint128_t g_lehmer64_state;


/**
 * Seeds the random number generator.
 */
static inline void random_seed(unsigned int seed)
{
    g_lehmer64_state = seed;
}

/**
 * A *fast* pseudo-random number generator (Lehmer's generator). Returns a random 64 bit number.
 */
static inline uint64_t lehmer64() {
    g_lehmer64_state *= 0xda942042e4dd58b5;
    return g_lehmer64_state >> 64;
}

/**
 * Generates and returns a random double in the range [0, 1), using Lehmer's generator.
 */
static inline double random_double_0_1_exc()
{
    // We cannot divide by 2^64, gcc complains that "integer constant is too large for its type".
    // So instead we shift the value right by one, and divide by 2^63.
    uint64_t val64 = lehmer64() >> 1;
    return val64 / (long double)0x8000000000000000;
}

/**
 * Generates and returns a random double in the range [0, 1], using Lehmer's generator.
 */
static inline double random_double_0_1_inc()
{
    uint64_t val64 = lehmer64();
    return (long double)val64 / 0xFFFFFFFFFFFFFFFF;
}

// /**
//  * Seeds the random number generator.
//  */
// static inline void random_seed(unsigned int seed)
// {
//     srand(seed);
// }

// /**
//  * Generates and returns a random double in the range [0, 1), using C built-in rand().
//  */
// static inline double random_double_0_1_exc()
// {
//     return ((double)rand()) / (RAND_MAX + 1.0);
// }

// /**
//  * Generates and returns a random double in the range [0, 1], using C built-in rand().
//  */
// static inline double random_double_0_1_inc()
// {
//     return ((double)rand()) / RAND_MAX;
// }

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
