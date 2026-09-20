/* AprilTag config.
 *
 * When APRILTAG_HAVE_CONFIG is defined, includes a user-provided apriltag_config.h
 * that can override allocators, assert, and feature macros. Without it, the library
 * compiles identically to upstream.
 */
#ifndef __APRILTAG_LIB_DEFAULT_CONFIG_H__
#define __APRILTAG_LIB_DEFAULT_CONFIG_H__

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#ifdef APRILTAG_HAVE_CONFIG
#include "apriltag_config.h"
#endif

// Features
#ifndef APRILTAG_ENABLE_PTHREADS
#define APRILTAG_ENABLE_PTHREADS            (1)
#endif
#ifndef APRILTAG_ENABLE_IMAGE_IO
#define APRILTAG_ENABLE_IMAGE_IO            (1)
#endif
#ifndef APRILTAG_ENABLE_DEBUG
#define APRILTAG_ENABLE_DEBUG               (1)
#endif
#ifndef APRILTAG_ENABLE_PROFILE
#define APRILTAG_ENABLE_PROFILE             (1)
#endif
#ifndef APRILTAG_ENABLE_TAG_NAMES
#define APRILTAG_ENABLE_TAG_NAMES           (1)
#endif
#ifndef APRILTAG_ENABLE_8_CONNECTIVITY
#define APRILTAG_ENABLE_8_CONNECTIVITY      (1)
#endif
#ifndef APRILTAG_ENABLE_32BIT_UNIONFIND
#define APRILTAG_ENABLE_32BIT_UNIONFIND     (1)
#endif

// Tag families.
#ifndef APRILTAG_ENABLE_TAG16H5
#define APRILTAG_ENABLE_TAG16H5             (1)
#endif
#ifndef APRILTAG_ENABLE_TAG25H9
#define APRILTAG_ENABLE_TAG25H9             (1)
#endif
#ifndef APRILTAG_ENABLE_TAG36H10
#define APRILTAG_ENABLE_TAG36H10            (1)
#endif
#ifndef APRILTAG_ENABLE_TAG36H11
#define APRILTAG_ENABLE_TAG36H11            (1)
#endif
#ifndef APRILTAG_ENABLE_TAGCIRCLE21H7
#define APRILTAG_ENABLE_TAGCIRCLE21H7       (1)
#endif
#ifndef APRILTAG_ENABLE_TAGCIRCLE49H12
#define APRILTAG_ENABLE_TAGCIRCLE49H12      (1)
#endif
#ifndef APRILTAG_ENABLE_TAGCUSTOM48H12
#define APRILTAG_ENABLE_TAGCUSTOM48H12      (1)
#endif
#ifndef APRILTAG_ENABLE_TAGSTANDARD41H12
#define APRILTAG_ENABLE_TAGSTANDARD41H12    (1)
#endif
#ifndef APRILTAG_ENABLE_TAGSTANDARD52H13
#define APRILTAG_ENABLE_TAGSTANDARD52H13    (1)
#endif

// Memory allocation ===
#ifndef apriltag_malloc
#define apriltag_malloc(s)      malloc(s)
#endif
#ifndef apriltag_calloc
#define apriltag_calloc(n, s)   calloc(n, s)
#endif
#ifndef apriltag_realloc
#define apriltag_realloc(p, s)  realloc(p, s)
#endif
#ifndef apriltag_free
#define apriltag_free(p)        free(p)
#endif
#ifndef apriltag_assert
#include <assert.h>
#define apriltag_assert(x)      assert(x)
#endif
#ifndef apriltag_poll_events
#define apriltag_poll_events()  ((void) 0)
#endif

// strdup uses malloc internally — default impl uses apriltag_malloc.
#ifndef apriltag_strdup
static inline char *apriltag_strdup(const char *s) {
    size_t len = strlen(s) + 1;
    char *d = (char *) apriltag_malloc(len);
    if (d) memcpy(d, s, len);
    return d;
}
#endif

// Returns the number of stack bytes safely available at the current call depth.
// ptsort divides this by sizeof(struct pt) to get the maximum number of elements
// it can allocate on the stack; anything larger goes to the heap.
#ifndef apriltag_stack_avail
static inline size_t apriltag_stack_avail(void) {
    return 4096;
}
#endif

#endif // __APRILTAG_LIB_DEFAULT_CONFIG_H__
