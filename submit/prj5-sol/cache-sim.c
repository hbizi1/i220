#include "cache-sim.h"

#include "memalloc.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>


/************************** Type Definitions  **************************/


//TODO: add auxiliary typedefs, enums, structs as needed

struct CacheSimImpl {
  //TODO
  char REMOVE_ME;
};

/******************** Creation / Destruction Routines ******************/

/** Create and return a new cache-simulation structure for a
 *  cache for main memory with the specified cache parameters params.
 *  No requirement that *params remains valid after this call.
 */
CacheSim *
new_cache_sim(const CacheParams *params)
{
  //TODO
  CacheSim *cache = NULL;
  return cache;
}

/** Free all resources used by cache-simulation structure *cache */
void
free_cache_sim(CacheSim *cache)
{
  //TODO
}

/************************* Simulation Routine **************************/


//TODO: add auxiliary function definitions as needed


/** Return result for reading (is_write == false) or writing (is_write == true)
 *  access_addr from cache
 */
CacheResult
cache_sim_result(CacheSim *cache, MemAddr access_addr, bool is_write)
{
  //TODO
  CacheResult result = { 0 };
  return result;
}
