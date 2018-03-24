//
// Created by sunside on 10.03.18.
//

#ifndef PROJECT_MAP_RESULT_T_H
#define PROJECT_MAP_RESULT_T_H

#include <any>

namespace firestorm {

    /// \brief The result of a mapping operation.
    class map_result_t {
    public:
        virtual ~map_result_t() noexcept = default;

        virtual std::any as_any() const noexcept = 0;

        template<class T>
        inline T any_cast() const {
            // TODO: Can we specialize this?
            return std::any_cast<T>(as_any());
        }
    };

    using map_result = std::shared_ptr<map_result_t>;

}

#endif //PROJECT_MAP_RESULT_T_H
