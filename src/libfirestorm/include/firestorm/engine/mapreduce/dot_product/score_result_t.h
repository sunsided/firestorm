//
// Created by sunside on 24.03.18.
//

#ifndef PROJECT_SCORE_RESULT_T_H
#define PROJECT_SCORE_RESULT_T_H

#include <vector>
#include <firestorm/engine/types/score_t.h>
#include <firestorm/engine/mapreduce/combine_result_t.h>
#include "firestorm/engine/mapreduce/map_result_t.h"
#include "firestorm/engine/mapreduce/reduce_result_t.h"

namespace firestorm {

    class score_result_t final : public map_result_t,
                                 public combine_result_t,
                                 public reduce_result_t,
                                 public typed_result_t<std::vector<score_t>> {
    public:
        explicit score_result_t(std::vector<score_t> scores) noexcept
            : _scores{std::move(scores)}
        {}

        const std::vector<score_t>& get() const noexcept final {
            return _scores;
        }

    private:
        std::vector<score_t> _scores;
    };

}

#endif //PROJECT_SCORE_RESULT_T_H
