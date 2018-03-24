//
// Created by sunside on 23.03.18.
//

#ifndef PROJECT_COMBINE_RESULT_T_H
#define PROJECT_COMBINE_RESULT_T_H

#include <any>

namespace firestorm {

    /// \brief The result of a combining operation
    class combine_result_t {
    public:
        virtual ~combine_result_t() noexcept = default;

        virtual std::any as_any() const noexcept = 0;

        template<class T>
        inline T any_cast() const {
            // TODO: Can we specialize this?
            return std::any_cast<T>(as_any());
        }
    };

    using combine_result = std::shared_ptr<combine_result_t>;

}

#endif //PROJECT_COMBINE_RESULT_T_H
