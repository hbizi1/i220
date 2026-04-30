#ifndef ACTIVE_MEM_H_
#define ACTIVE_MEM_H_

#include <stdbool.h>
#include <stddef.h>

typedef unsigned long long MemVal;

/** callback for an attempt made to read addr.  Returned value is what
 *  the read should return.
 */
typedef MemVal MemReadFn(void *ctx, const void *addr);

/** callback for an attempt made to write addr with val. */
typedef void MemWriteFn(void *ctx, const void *addr, MemVal val);

/** allocate n_bytes of active memory with read attempts resulting in
 *  calls to read_fn and write attempts resulting in calls to write_fn.
 *  Returns NULL on failure.
 */
void *active_mem_calloc(size_t n_bytes, void *ctx,
                        MemReadFn *read_fn,  MemWriteFn *write_Fn);


/** free previously allocated n_bytes of active memory at p.  Returns
 *  non-zero on error.
 */
int active_mem_free(void *p, size_t n_bytes);

#endif //#ifndef ACTIVE_MEM_H_
