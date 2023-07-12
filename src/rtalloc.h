#ifndef __RTALLOC_H__
#define __RTALLOC_H__

#include <stdio.h>
#include <stdlib.h>


/**
 * Wrappers around malloc()/free() heap memory allocators.
 *
 * We call these (instead of calling malloc()/free() directly) just in case we will decide to replace the standard malloc()/free() with
 * some other heap memory allocator later on - the we would only need to replace them here.
 */


#define rtfree(p)           free(p)


static inline void * rtalloc(size_t sz);


static inline void * rtalloc(size_t sz)
{
    void *p = malloc(sz);
    if (p == NULL) {
        fprintf(stderr, "Error: could not allocate heap memory. Exiting.");
        exit(1);
    }
    return p;
}

#endif // __RTALLOC_H__
