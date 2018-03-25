//
// Created by sunside on 23.03.18.
//

#ifndef PROJECT_COMBINER_T_H
#define PROJECT_COMBINER_T_H

#include <memory>
#include <firestorm/engine/mapreduce/map_result_t.h>
#include "combine_result_t.h"

namespace firestorm {

    /// \brief Interface to a combiner type.
    class combine_t {
    public:
        virtual ~combine_t() = default;

        /// \brief Combines the results of mappers.
        /// \param other The other visitor to merge into the local results.
        virtual void combine(const map_result& other) = 0;
    };

    /// \brief A stateful class that performs mapping result combination.
    class combiner_t : public virtual combine_t {
    public:
        ~combiner_t() override = default;

        /// \brief Initializes a combine operation.
        virtual void begin() = 0;

        /// \brief Finalizes a combine operation.
        /// \return A combine result.
        virtual combine_result finish() = 0;
    };

    /// \brief Pointer to a combiner.
    using combiner_ptr = std::shared_ptr<combiner_t>;

    /// \brief Interface to a concretely typed combiner type.
    template<typename T>
    class typed_combine_t : public virtual combine_t {

        static_assert(std::is_convertible<T*, map_result_t*>::value,
                      "Derived type must inherit map_result_t as public");
    public:
        ~typed_combine_t() override = default;

        /// \brief Reduces the results of mappers.
        /// \param other The other visitor to merge into the local results.
        inline void combine(const map_result& other) final {
            const auto cast = dynamic_cast<const T*>(other.get());
            assert (cast != nullptr);
            combine(*cast);
        }

        /// \brief Combines the results of mappers.
        /// \param other The other visitor to merge into the local results.
        virtual void combine(const T& other) = 0;

        /// \brief Combines the results of mappers.
        /// \param other The other visitor to merge into the local results.
        inline void combine(const std::shared_ptr<T>& other) {
            combine(*other);
        }
    };

    /// \brief A stateful class that performs mapping result combination.
    template<typename T>
    class typed_combiner_t :
            public virtual combiner_t,
            public virtual typed_combine_t<T> {
    public:
        ~typed_combiner_t() override = default;
    };

}

#endif //PROJECT_COMBINER_T_H
