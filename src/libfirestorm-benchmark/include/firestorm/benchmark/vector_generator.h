//
// Created by sunside on 22.03.18.
//

#ifndef PROJECT_VECTOR_GENERATOR_H
#define PROJECT_VECTOR_GENERATOR_H

#include <cstdlib>
#include <memory>
#include <firestorm/engine/types/vector_t.h>

namespace firestorm {

    class vector_generator {
    public:
        explicit vector_generator(std::size_t dimensions) noexcept;
        vector_generator(std::size_t dimensions, std::size_t seed) noexcept;
        ~vector_generator() noexcept;

        vector_ptr create_vector();
        vector_ptr create_vector(size_t seed);

        void fill_vector(float* target);
        void fill_vector(float* target, size_t seed);

    private:
        class Impl;
        std::unique_ptr<Impl> _impl;
    };

}

#endif //PROJECT_VECTOR_GENERATOR_H
