//
// Created by sunside on 10.03.18.
//

#ifndef PROJECT_REDUCE_RESULT_T_H
#define PROJECT_REDUCE_RESULT_T_H

#include <any>
#include "any_result_t.h"

namespace firestorm {

    /// \brief The result of a reducing operation
    class reduce_result_t : public virtual any_result_t {
    public:
        virtual ~reduce_result_t() noexcept = default;
    };

    using reduce_result = std::shared_ptr<reduce_result_t>;

}

#endif //PROJECT_REDUCE_RESULT_T_H
