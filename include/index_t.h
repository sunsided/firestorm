//
// Created by Markus on 27.05.2017.
//

#ifndef FIRESTORM_INDEX_T_H
#define FIRESTORM_INDEX_T_H

#include <memory>

/// Data type describing the index of a chunk.
///
/// The underlying type size is selected to provide
/// a tradeoff between value range and runtime speed
/// due to address size and data type compatibility.
/// Ideally, size_t would be preferred, but this might
/// require too much memory per index entry.
using chunk_idx_t = uint32_t;

/// Data type describing the index of a vector in a chunk.
///
/// Assuming that a chunk consists of many one-byte items.
/// up to 4.294.967.295 separate items can be encoded
/// using a 32-bit unsigned integer, which would imply
/// using 4 GiB chunk sizes.
using vector_idx_t = uint32_t;

/// An index entry.
struct index_t {
    /// Identifies the chunk in the manager.
    const chunk_idx_t chunk;
    /// Identifies the index in the chunk.
    const vector_idx_t index;

    constexpr index_t(const chunk_idx_t chunk, const vector_idx_t index)
            : chunk(chunk), index(index)
    {}
};

#endif //FIRESTORM_INDEX_T_H
