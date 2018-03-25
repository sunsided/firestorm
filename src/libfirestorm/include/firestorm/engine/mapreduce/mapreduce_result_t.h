//
// Created by sunside on 24.03.18.
//

#ifndef PROJECT_ANY_RESULT_T_H
#define PROJECT_ANY_RESULT_T_H

#include <any>
#include <typeindex>

namespace firestorm {

    /// \brief Interface that allows for getting the typed value of a mapreduce result.
    /// \tparam T The type of the result.
    template<typename T>
    class has_result_t {
    public:
        virtual ~has_result_t() noexcept = default;

        /// \brief Gets the value of the result.
        /// \return The result.
        virtual const T& get() const noexcept = 0;

#if false
        /// \brief Gets the type of the result.
        /// \return The result type.
        inline std::type_index value_type() const { return typeid(T); };
#endif
    };

    /// \brief A generic mapreduce result.
    /// \note In concrete results, implement \c typed_mapreduce_result_t as well.
    /// \see typed_mapreduce_result_t
    class mapreduce_result_t {
    public:
        virtual ~mapreduce_result_t() noexcept = default;

        /// \brief Obtains an std::any representation of the result.
        /// \return The type-erased result.
        virtual std::any as_any() const noexcept = 0;

        /// \brief Attempts to cast the (untyped) result to a concrete type.
        /// \tparam T The type of the result.
        /// \return The result of the cast.
        template<typename T>
        inline T any_cast() const {
            const auto ptr = dynamic_cast<const has_result_t<T>*>(this);
            if (ptr != nullptr) {
                return ptr->get();
            }

            return std::any_cast<T>(as_any());
        }
    };

    /// \brief A typed mapreduce result.
    /// \tparam T The type of the result.
    template<typename T>
    class typed_mapreduce_result_t : public virtual mapreduce_result_t,
                                     public virtual has_result_t<T> {
    public:
        using value_type = T;

        ~typed_mapreduce_result_t() noexcept override = default;

        /// \brief Gets the value of the result.
        /// \return The result.
        const T& get() const noexcept override = 0;

        /// \brief Gets the result as an std::any.
        /// \return The type-erased result.
        inline std::any as_any() const noexcept final { return get(); }
    };

}

#endif //PROJECT_ANY_RESULT_T_H
