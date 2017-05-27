//
// Created by Markus on 27.05.2017.
//

#ifndef FIRESTORM_VECTOR_IDX_T_H
#define FIRESTORM_VECTOR_IDX_T_H

#include <memory>

/// Data type describing the index of a vector in a chunk.
///
/// Assuming that a chunk consists of many one-byte items.
/// up to 4.294.967.295 separate items can be encoded
/// using a 32-bit unsigned integer, which would imply
/// using 4 GiB chunk sizes.
using vector_idx_t = uint32_t;

#endif //FIRESTORM_VECTOR_IDX_T_H
