//
// Created by sunside on 10.03.18.
//

#ifndef PROJECT_REDUCE_RESULT_T_H
#define PROJECT_REDUCE_RESULT_T_H

#include <any>

namespace firestorm {

    /// \brief The result of a reducing operation
    class reduce_result_t {
    public:
        virtual ~reduce_result_t() noexcept = default;

        virtual std::any as_any() const noexcept = 0;

        template<class T>
        inline T any_cast() const {
            // TODO: Can we specialize this?
            return std::any_cast<T>(as_any());
        }
    };

    using reduce_result = std::shared_ptr<reduce_result_t>;

}

#endif //PROJECT_REDUCE_RESULT_T_H
