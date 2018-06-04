#pragma once
#include "compiler.h"
#include "endian.h"
#include "wshash.h"
#include <stdio.h>

#define ENABLE_SSE 0

#if defined(_M_X64) && ENABLE_SSE
#include <smmintrin.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

// compile time settings
#define NODE_WORDS (64/4)
#define MIX_WORDS (WSHASH_MIX_BYTES/4)
#define MIX_NODES (MIX_WORDS / NODE_WORDS)
#include <stdint.h>

typedef union node {
	uint8_t bytes[NODE_WORDS * 4];
	uint32_t words[NODE_WORDS];
	uint64_t double_words[NODE_WORDS / 2];

#if defined(_M_X64) && ENABLE_SSE
	__m128i xmm[NODE_WORDS/4];
#endif

} node;

static inline void wshash_h256_reset(wshash_h256_t* hash)
{
	memset(hash, 0, 32);
}

struct wshash_light {
	void* cache;
	uint64_t cache_size;
	uint64_t block_number;
};

/**
 * Allocate and initialize a new wshash_light handler. Internal version
 *
 * @param cache_size    The size of the cache in bytes
 * @param seed          Block seedhash to be used during the computation of the
 *                      cache nodes
 * @return              Newly allocated wshash_light handler or NULL in case of
 *                      ERRNOMEM or invalid parameters used for @ref wshash_compute_cache_nodes()
 */
wshash_light_t wshash_light_new_internal(uint64_t cache_size, wshash_h256_t const* seed);

/**
 * Calculate the light client data. Internal version.
 *
 * @param light          The light client handler
 * @param full_size      The size of the full data in bytes.
 * @param header_hash    The header hash to pack into the mix
 * @param nonce          The nonce to pack into the mix
 * @return               The resulting hash.
 */
wshash_return_value_t wshash_light_compute_internal(
	wshash_light_t light,
	uint64_t full_size,
	wshash_h256_t const header_hash,
	uint64_t nonce
);

void wshash_calculate_dag_item(
	node* const ret,
	uint32_t node_index,
	wshash_light_t const cache
);

uint64_t wshash_get_datasize(uint64_t const block_number);
uint64_t wshash_get_cachesize(uint64_t const block_number);

#ifdef __cplusplus
}
#endif
