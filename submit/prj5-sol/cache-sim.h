#ifndef CACHE_SIM_
#define CACHE_SIM_

#include <stdbool.h>

/** Opaque implementation */
typedef struct CacheSimImpl CacheSim;

/** Replacement strategy */
typedef enum {
  LRU_R,         /** Least Recently Used */
  MRU_R,         /** Most Recently Used */
  RANDOM_R       /** Random replacement */
} Replacement;

/** A primary memory address */
typedef unsigned long MemAddr;

/** Parameters which specify a cache.
 *  Must have n_set_index_bits + n_blk_offset_bits < n_mem_addr_bits and
 *  2 <= n_blk_offset_bits.
 */
typedef struct {
  unsigned n_mem_addr_bits;   // slides notation: m; # of bits in mem
                              // addr; total memory space is 2**this
  unsigned n_set_index_bits;  // slides notation: s; # of sets is 2**this
  unsigned n_blk_offset_bits; // slides notation: b; # of bits for offset
                              // within a block; blk-size is 2**this
  unsigned n_lines_per_set;   // Slides notation: E; # of cache lines/set
  Replacement replacement;    // replacement strategy
} CacheParams;


/** Create and return a new cache-simulation structure for a
 *  cache for main memory with the specified cache parameters params.
 *  No requirement that *params remains valid after this call.
 */
CacheSim *new_cache_sim(const CacheParams *params);

/** Free all resources used by cache-simulation structure *cache */
void free_cache_sim(CacheSim *cache);

typedef enum {
  CACHE_HIT,                     // address found in cache
  CACHE_MISS_WITHOUT_REPLACE,    // cache miss, no line replaced
  CACHE_MISS_WITH_REPLACE,       // cache miss, line replaced
  CACHE_N_STATUS                 // dummy value: # of status values possible
} CacheStatus;

typedef struct {
  MemAddr access_addr;  // address which was accessed.
  CacheStatus status;   // status of requested address
  MemAddr replace_addr; // address of replaced block if status is
                        // CACHE_MISS_WITH_REPLACE
  bool is_dirty;        // true if replaced block was dirty
} CacheResult;

/** Return result for reading (is_write == false) or writing (is_write == true)
 *  access_addr from cache
 */
CacheResult cache_sim_result(CacheSim *cache, MemAddr access_addr,
                             bool is_write);

#endif //ifndef CACHE_SIM_
