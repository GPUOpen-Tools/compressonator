// This file is part of meshoptimizer library; see meshoptimizer.h for version/license details
#include "meshoptimizer.h"

#include <assert.h>
#include <vector>

meshopt_VertexFetchStatistics meshopt_analyzeVertexFetch(const unsigned int* indices, size_t index_count, size_t vertex_count, size_t vertex_size, size_t cache_line_size, size_t cache_total_size) {
    assert(index_count % 3 == 0);
    assert(vertex_size > 0 && vertex_size <= 256);

    meshopt_VertexFetchStatistics result = {};

    const size_t kCacheLine = cache_line_size;
    const size_t kCacheSize = cache_total_size;

    // simple direct mapped cache; on typical mesh data this is close to 4-way cache, and this model is a gross approximation anyway
    std::vector<size_t> cache;
    cache.resize(kCacheSize / kCacheLine);

    for (size_t i = 0; i < index_count; ++i) {
        unsigned int index = indices[i];
        assert(index < vertex_count);

        size_t start_address = index * vertex_size;
        size_t end_address = start_address + vertex_size;

        size_t start_tag = start_address / kCacheLine;
        size_t end_tag = (end_address + kCacheLine - 1) / kCacheLine;

        assert(start_tag < end_tag);

        for (size_t tag = start_tag; tag < end_tag; ++tag) {
            size_t line = tag % (cache.size());

            // we store +1 since cache is filled with 0 by default
            result.bytes_fetched += (cache[line] != tag + 1) * kCacheLine;
            cache[line] = tag + 1;
        }
    }

    result.overfetch = vertex_count == 0 ? 0 : float(result.bytes_fetched) / float(vertex_count * vertex_size);

    return result;
}
