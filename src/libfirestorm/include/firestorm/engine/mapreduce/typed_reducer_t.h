//
// Created by sunside on 25.03.18.
//

#ifndef PROJECT_TYPED_REDUCER_T_H
#define PROJECT_TYPED_REDUCER_T_H

#include <cassert>
#include <any>
#include <memory>
#include <type_traits>
#include "reducer_t.h"

namespace firestorm {

    /// \brief Interface to a concretely typed reducer type.
    template<typename T>
    class typed_reduce_t : public virtual reduce_t {

        static_assert(std::is_convertible<T*, combine_result_t*>::value,
                      "Derived type must inherit combine_result_t as public");
        static_assert(std::is_convertible<T*, reduce_result_t*>::value,
                      "Derived type must inherit reduce_result_t as public");
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
            assert(cast != nullptr);
            reduce(*cast);
        }

        /// \brief Reduces the results of two combiners.
        /// \param other The result to merge into the local results.
        virtual void reduce(const T& other) = 0;

        /// \brief Reduces the results of two combiners.
        /// \param other The result to merge into the local results.
        inline void reduce(const std::shared_ptr<T>& other) {
            reduce(*other);
        }
    };

    /// \brief Interface to a concretely typed reducer type.
    template<typename T, typename U>
    class typed_reduce2_t : public virtual reduce_t {

        static_assert(std::is_convertible<T*, combine_result_t*>::value,
                      "Derived type must inherit combine_result_t as public");
        static_assert(std::is_convertible<U*, reduce_result_t*>::value,
                      "Derived type must inherit reduce_result_t as public");
    public:
        ~typed_reduce2_t() override = default;

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
            const auto cast = dynamic_cast<const U*>(other.get());
            assert(cast != nullptr);
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

    /// \brief A stateful class that performs combination result reduction.
    template<typename T>
    class typed_reducer_t:
            public virtual reducer_t,
            public virtual typed_reduce_t<T> {
    public:
        ~typed_reducer_t() override = default;
    };

    /// \brief A stateful class that performs combination result reduction.
    template<typename T, typename U>
    class typed_reducer2_t:
            public virtual reducer_t,
            public virtual typed_reduce2_t<T, U> {
    public:
        ~typed_reducer2_t() override = default;
    };

}


#endif //PROJECT_TYPED_REDUCER_T_H
