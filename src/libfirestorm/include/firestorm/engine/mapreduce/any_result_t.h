//
// Created by sunside on 24.03.18.
//

#ifndef PROJECT_ANY_RESULT_T_H
#define PROJECT_ANY_RESULT_T_H

#include <any>
#include <typeindex>

namespace firestorm {

    template<typename T>
    class has_result_t {
    public:
        virtual ~has_result_t() noexcept = default;

        virtual const T& get() const noexcept = 0;

        inline std::type_index value_type() const { return typeid(T); };
    };

    class any_result_t {
    public:
        virtual ~any_result_t() noexcept = default;

        virtual std::any as_any() const noexcept = 0;

        template<typename T>
        inline T any_cast() const {
            const auto ptr = dynamic_cast<const has_result_t<T>*>(this);
            if (ptr != nullptr) {
                return ptr->get();
            }

            return std::any_cast<T>(as_any());
        }
    };

    template<typename T>
    class typed_result_t : public virtual any_result_t,
                           public virtual has_result_t<T> {
    public:
        ~typed_result_t() noexcept override = default;

        const T& get() const noexcept override = 0;

        inline std::any as_any() const noexcept final { return get(); }
    };

}

#endif //PROJECT_ANY_RESULT_T_H
