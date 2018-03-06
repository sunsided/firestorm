//
// Created by Markus on 27.05.2017.
//

#ifndef FIRESTORM_CHUNK_IDX_T_H
#define FIRESTORM_CHUNK_IDX_T_H

#include <memory>

namespace firestorm {

/// Data type describing the index of a chunk.
///
/// The underlying type size is selected to provide
/// a tradeoff between value range and runtime speed
/// due to address size and data type compatibility.
/// Ideally, size_t would be preferred, but this might
/// require too much memory per index entry.
    using chunk_idx_t = uint32_t;

}

#endif //FIRESTORM_CHUNK_IDX_T_H
