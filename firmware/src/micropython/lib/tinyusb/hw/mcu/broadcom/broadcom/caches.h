
#pragma once

#include <stddef.h>

void init_caches(void);

// Writes values from the cache back into memory but keep a copy in the cache.
void data_clean(volatile void* starting_address, size_t size);

// Writes values from the cache back into memory and remove it from the cache.
void data_clean_and_invalidate(volatile void* starting_address, size_t size);

// Remove values from the cache because the value in memory may have changed.
void data_invalidate(volatile void* starting_address, size_t size);
