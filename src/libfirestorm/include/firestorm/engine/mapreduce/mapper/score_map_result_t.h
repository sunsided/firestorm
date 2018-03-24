//
// Created by sunside on 24.03.18.
//

#ifndef PROJECT_SCORE_MAP_RESULT_T_H
#define PROJECT_SCORE_MAP_RESULT_T_H

#include <firestorm/engine/mapreduce/map_result_t.h>
#include <firestorm/engine/types/score_t.h>
#include <utility>
#include <vector>

namespace firestorm {

    class score_map_result_t final : public map_result_t {
    public:
        explicit score_map_result_t(std::vector<score_t> scores) noexcept
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

#endif //PROJECT_SCORE_MAP_RESULT_T_H
