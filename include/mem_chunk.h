//
// Created by Markus on 27.05.2017.
//

#ifndef FIRESTORM_MEM_CHUNK_H
#define FIRESTORM_MEM_CHUNK_H

#include <boost/align/aligned_alloc.hpp>
#include <units.h>

struct mem_chunk_t {
    static const size_t byte_alignment = 32;
    float* data;

    mem_chunk_t(bytes_t bytes) {
        data = reinterpret_cast<float*>(boost::alignment::aligned_alloc(byte_alignment, bytes));
    }

    ~mem_chunk_t() {
        boost::alignment::aligned_free(data);
        data = nullptr;
    }
};

#endif //FIRESTORM_MEM_CHUNK_H
