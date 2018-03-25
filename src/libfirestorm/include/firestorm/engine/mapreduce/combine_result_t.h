//
// Created by sunside on 23.03.18.
//

#ifndef PROJECT_COMBINE_RESULT_T_H
#define PROJECT_COMBINE_RESULT_T_H

#include <any>
#include "mapreduce_result_t.h"

namespace firestorm {

    /// \brief The result of a combining operation
    class combine_result_t : public virtual mapreduce_result_t {
    public:
        virtual ~combine_result_t() noexcept = default;
    };

    using combine_result = std::shared_ptr<combine_result_t>;

}

#endif //PROJECT_COMBINE_RESULT_T_H
