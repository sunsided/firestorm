//
// Created by sunside on 22.03.18.
//

#include <functional>
#include <random>
#include <firestorm/benchmark/vector_generator.h>
#ifdef USE_OPENMP
#include <firestorm/engine/vector_ops/dot_product_openmp.h>
#else
#include <firestorm/engine/vector_ops/dot_product_naive.h>
#endif

namespace firestorm {

    class vector_generator::Impl {
    public:
        Impl(const std::size_t dimensions) noexcept
                : _dimensions{dimensions}, _generator{}
        { }

        Impl(const std::size_t dimensions, const std::size_t seed) noexcept
            : _dimensions{dimensions}, _generator{seed}
        { }

        inline vector_ptr create_vector() {
            return create_vector(_generator);
        }

        inline vector_ptr create_vector(size_t seed) {
            std::default_random_engine generator(seed);
            return create_vector(generator);
        }

        void fill_vector(float* target) {
            create_vector(target, _generator);
        }

        void fill_vector(float* target, size_t seed) {
            std::default_random_engine generator(seed);
            create_vector(target, generator);
        }

    private:
        vector_ptr create_vector(std::default_random_engine& generator) {
            vector_t query{_dimensions};
            create_vector(query.data, generator);
            return std::make_shared<vector_t>(std::move(query));
        }

        void create_vector(float* target, std::default_random_engine& generator) {
            for (size_t i = 0; i < _dimensions; ++i) {
                target[i] = _distribution(generator);
            }

#ifdef USE_OPENMP
            vec_normalize_openmp(target, _dimensions);
#else
            vec_normalize_naive(target, _dimensions);
#endif
        }

    private:
        const size_t _dimensions;
        std::default_random_engine _generator;
        std::normal_distribution<float> _distribution{0.0f, 2.0f};
    };

    vector_generator::vector_generator(const std::size_t dimensions) noexcept
            : _impl{std::make_unique<vector_generator::Impl>(dimensions)}
    {}

    vector_generator::vector_generator(const std::size_t dimensions, const std::size_t seed) noexcept
        : _impl{std::make_unique<vector_generator::Impl>(dimensions, seed)}
    {}

    vector_generator::~vector_generator() = default;

    vector_ptr vector_generator::create_vector() {
        return _impl->create_vector();
    }

    vector_ptr vector_generator::create_vector(const size_t seed) {
        return _impl->create_vector(seed);
    }

    void vector_generator::fill_vector(float* target) {
        _impl->fill_vector(target);
    }

    void vector_generator::fill_vector(float* target, size_t seed) {
        _impl->fill_vector(target, seed);
    }

}