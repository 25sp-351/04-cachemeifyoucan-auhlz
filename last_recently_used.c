#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "cache.h"
#include "vec.h"

// "last recently used" NOT "least recently used" :grin:

int cache_requests           = 0;
int cache_hits               = 0;

KeyType   last_key           = -1;
ValueType last_value         = NULL;

ProviderFunction _downstream = NULL;


CacheStat* statistics(void) {
    fprintf(stderr, __FILE__ " statistics()\n");

    CacheStat* current_stats = malloc(3 * sizeof(CacheStat));
    current_stats[0]         = (CacheStat){Cache_requests, cache_requests};
    current_stats[1]         = (CacheStat){Cache_hits, cache_hits};
    current_stats[2]         = (CacheStat){END_OF_STATS, 0};

    return current_stats;
}


void reset_statistics(void) {
    fprintf(stderr, __FILE__ " reset_statistics()\n");
    cache_requests = 0;
    cache_hits     = 0;
}


ValueType _caching_provider(KeyType key) {
    cache_requests++;

    if (key == last_key) {
        cache_hits++;
        return last_value;
    }

    ValueType result = (*_downstream)(key);
    last_key         = key;
    last_value       = result;

    return result;
}


ProviderFunction set_provider(ProviderFunction downstream) {
    fprintf(stderr, __FILE__ " set_provider()\n");
    _downstream = downstream;
    return _caching_provider;
}