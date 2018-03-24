//
// Created by sunside on 10.03.18.
//

#ifndef PROJECT_MAP_RESULT_T_H
#define PROJECT_MAP_RESULT_T_H

#include <any>
#include "any_result_t.h"

namespace firestorm {

    /// \brief The result of a mapping operation.
    class map_result_t : public virtual any_result_t {
    public:
        virtual ~map_result_t() noexcept = default;
    };

    using map_result = std::shared_ptr<map_result_t>;

}

#endif //PROJECT_MAP_RESULT_T_H
