#include <assert.h>
#include <locale.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "main.h"

#include "random.h"
#include "renderer.h"
#include "vector.h"


void test_random_point_in_unit_sphere_algorithms_performance();
void test_random_point_in_unit_sphere__random_algo(Vector3 *pArr, uint64_t iterations);
void test_random_point_in_unit_sphere__trigonometric_algo(Vector3 *pArr, uint64_t iterations);


bool debug = DEBUG;
#define DEBUG_ANGLE_TO_CC   debug && 1

#define dbg(...) if (debug) { printf(__VA_ARGS__); }

#define assert2(exp, msg) if (! (exp)) { printf("%s", msg); exit(1); }


double piDiv2 = (double)PI / 2;


#if defined(ENV_LINUX) && ENV_LINUX
int main()
#else
int WinMain()
#endif // ENV_LINUX
{
    App app;

    init_app(&app);

    test_random_point_in_unit_sphere_algorithms_performance();
    return 0;
}

void test_random_point_in_unit_sphere_algorithms_performance()
{
    uint64_t iterations = 10000000;
    Vector3 *pArr = malloc(sizeof(Vector3) * iterations);
    if (pArr == NULL) {
        log_err("Fatal error: could not allocate pArr. Out of memory?");
        exit(1);
    }

    test_random_point_in_unit_sphere__random_algo(pArr, iterations);
    test_random_point_in_unit_sphere__trigonometric_algo(pArr, iterations);

    // Print out a random point from the generated array, to make sure the actual random point generation is not removed by compiler optimization.
    Vector3 *p = &pArr[random_int_exc(0, iterations)];
    printf("random point = {.x = %f, .y = %f, .z = %f}\n", p->x, p->y, p->z);
    printf("vector3_length(random point) = %f\n", vector3_length(p));
}

void test_random_point_in_unit_sphere__random_algo(Vector3 *pArr, uint64_t iterations)
{
    struct timespec tstart, tend;
    clock_gettime(CLOCK_MONOTONIC, &tstart);

    for (uint64_t i = 0; i < iterations; i++) {
        random_point_in_unit_sphere__random_algo(&pArr[i]);
    }

    clock_gettime(CLOCK_MONOTONIC, &tend);
    double total_time = (tend.tv_sec - tstart.tv_sec) + ((tend.tv_nsec - tstart.tv_nsec) / 1000000000.0);
    double iter_time = total_time / iterations;

    printf("Random algo:\n");
    printf("Total time         = %.9f\n", total_time);
    printf("Time per iteration = %.9f\n", iter_time);
    printf("\n");
}

void test_random_point_in_unit_sphere__trigonometric_algo(Vector3 *pArr, uint64_t iterations)
{
    struct timespec tstart, tend;
    clock_gettime(CLOCK_MONOTONIC, &tstart);

    for (uint64_t i = 0; i < iterations; i++) {
        random_point_in_unit_sphere__trigonometric_algo(&pArr[i]);
    }

    clock_gettime(CLOCK_MONOTONIC, &tend);
    double total_time = (tend.tv_sec - tstart.tv_sec) + ((tend.tv_nsec - tstart.tv_nsec) / 1000000000.0);
    double iter_time = total_time / iterations;

    printf("Trigonometric algo:\n");
    printf("Total time         = %.9f\n", total_time);
    printf("Time per iteration = %.9f\n", iter_time);
    printf("\n");
}

void log_err(char *err)
{
    fprintf(stderr, "%s", err);
}

void init_app(App *app)
{
    setbuf(stdout, NULL);   // Disable stdout buffering.

    setlocale(LC_NUMERIC, "");  // Set numeric locale, to get printf("%'f") to separate thousands in numbers with commas ","

    // Seed the random number generator.
    struct timespec tnow;
    clock_gettime(CLOCK_MONOTONIC, &tnow);
    random_seed(tnow.tv_nsec);

    app->window = NULL;
    app->renderer = NULL;
}
