//
// Created by sunside on 24.03.18.
//

#ifndef PROJECT_SCORE_COMBINE_RESULT_T_H
#define PROJECT_SCORE_COMBINE_RESULT_T_H

#include <vector>
#include <firestorm/engine/types/score_t.h>
#include <firestorm/engine/mapreduce/combine_result_t.h>

namespace firestorm {

    class score_combine_result_t final : public combine_result_t {
    public:
        explicit score_combine_result_t(std::vector<score_t> scores) noexcept
            : _scores{std::move(scores)}
        {}

        inline const std::vector<score_t>& get() noexcept {
            return _scores;
        }

        inline std::any as_any() const noexcept final {
            return _scores;
        }

    private:
        std::vector<score_t> _scores;
    };

}

#endif //PROJECT_SCORE_COMBINE_RESULT_T_H
