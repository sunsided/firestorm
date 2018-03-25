//
// Created by Markus on 27.05.2017.
//

#ifndef FIRESTORM_REDUCER_T_H
#define FIRESTORM_REDUCER_T_H

#include <iterator>
#include <any>
#include <memory>
#include <firestorm/engine/types/mem_chunk_t.h>
#include <firestorm/engine/types/vector_t.h>
#include <firestorm/engine/types/score_t.h>
#include <firestorm/engine/mapreduce/combine_result_t.h>
#include "reduce_result_t.h"

namespace firestorm {

    /// \brief Interface to a reducer type.
    class reduce_t {
    public:
        virtual ~reduce_t() = default;

        /// \brief Reduces the results of two combiners.
        /// \param other The result to merge into the local results.
        virtual void reduce(const combine_result& other) = 0;

        /// \brief Reduces the results of two reducers.
        /// \param other The result to merge into the local results.
        virtual void reduce(const reduce_result& other) = 0;
    };

    /// \brief A stateful class that performs mapping result reduction.
    class reducer_t : public reduce_t {
    public:
        ~reducer_t() override = default;

        /// \brief Initializes a reducing operation.
        virtual void begin() = 0;

        /// \brief Finalizes a reducing operation.
        /// \return The reducing result.
        virtual reduce_result finish() = 0;
    };

    /// \brief Pointer to a reducer.
    using reducer_ptr = std::shared_ptr<reducer_t>;

    /// \brief Interface to a concretely typed reducer type.
    template<typename T, typename U = T>
    class typed_reduce_t : public virtual reduce_t {

        static_assert(std::is_convertible<T*, combine_result_t*>::value,
                      "Derived type must inherit combine_result_t as public");
        static_assert(std::is_convertible<U*, combine_result_t*>::value,
                      "Derived type must inherit combine_result_t as public");
    public:
        ~typed_reduce_t() override = default;

        /// \brief Reduces the results of two combiners.
        /// \param other The result to merge into the local results.
        inline void reduce(const combine_result& other) final {
            const auto cast = dynamic_cast<const T*>(other.get());
            assert (cast != nullptr);
            reduce(*cast);
        }

        /// \brief Reduces the results of two reducers.
        /// \param other The result to merge into the local results.
        inline void reduce(const reduce_result& other) final {
            const auto cast = dynamic_cast<const T*>(other.get());
            assert (cast != nullptr);
            reduce(*cast);
        }

        /// \brief Reduces the results of two combiners.
        /// \param other The result to merge into the local results.
        virtual void reduce(const T& other) = 0;

        /// \brief Reduces the results of two reducers.
        /// \param other The result to merge into the local results.
        virtual void reduce(const U& other) = 0;

        /// \brief Reduces the results of two combiners.
        /// \param other The result to merge into the local results.
        inline void reduce(const std::shared_ptr<T>& other) {
            reduce(*other);
        }

        /// \brief Reduces the results of two reducers.
        /// \param other The result to merge into the local results.
        inline void reduce(const std::shared_ptr<U>& other) {
            reduce(*other);
        }
    };

    /// \brief A stateful class that performs mapping result combination.
    template<typename T>
    class typed_reducer_t :
            public virtual reducer_t,
            public virtual typed_reduce_t<T> {
    public:
        ~typed_reducer_t() override = default;
    };

}

#endif //FIRESTORM_REDUCER_T_H
