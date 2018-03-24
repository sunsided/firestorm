//
// Created by sunside on 24.03.18.
//

#ifndef PROJECT_ANY_RESULT_T_H
#define PROJECT_ANY_RESULT_T_H

#include <any>

namespace firestorm {

    class any_result_t {
    public:
        virtual ~any_result_t() noexcept = default;

        virtual std::any as_any() const noexcept = 0;

        template<class T>
        inline T any_cast() const {
            // TODO: Can we specialize this?
            return std::any_cast<T>(as_any());
        }
    };

    template<class T>
    class typed_result_t : public virtual any_result_t {
    public:
        ~typed_result_t() noexcept override = default;

        virtual const T& get() const noexcept = 0;

        inline std::any as_any() const noexcept final { return get(); }
    };

}

#endif //PROJECT_ANY_RESULT_T_H
