//
// Created by sunside on 23.03.18.
//

#ifndef PROJECT_KEEP_ALL_COMBINER_H
#define PROJECT_KEEP_ALL_COMBINER_H

#include <any>
#include <vector>
#include <firestorm/engine/types/score_t.h>
#include "firestorm/engine/mapreduce/combiner_t.h"
#include <firestorm/engine/mapreduce/dot_product/score_result_t.h>

namespace firestorm {

    class keep_all_combiner final : public combiner_t {
    public:
        keep_all_combiner() = default;
        ~keep_all_combiner() final = default;

        void begin() final {
            _scores.clear();
        }

        void combine(const map_result& other) final {
            auto other_scores = other->any_cast<std::vector<score_t>>();

            for (const auto &result : other_scores) {
                _scores.push_back(result);
            }

            other_scores.clear();
        }

        combine_result finish() final {
            return std::make_shared<score_result_t>(_scores);
        }

    private:
        std::vector<score_t> _scores;
    };

}

#endif //PROJECT_KEEP_ALL_COMBINER_H