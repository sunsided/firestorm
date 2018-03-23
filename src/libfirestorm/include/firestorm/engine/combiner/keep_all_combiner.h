//
// Created by sunside on 23.03.18.
//

#ifndef PROJECT_KEEP_ALL_COMBINER_H
#define PROJECT_KEEP_ALL_COMBINER_H

#include <any>
#include <vector>
#include <firestorm/engine/types/score_t.h>
#include "combiner_t.h"

namespace firestorm {

    class keep_all_combiner final : public combiner_t {
    public:
        keep_all_combiner() = default;
        ~keep_all_combiner() final = default;

        void begin() final {
            scores.clear();
        }

        void combine(const map_result_t& other) final {
            auto other_scores = std::any_cast<std::vector<score_t>>(other);

            for (const auto &result : other_scores) {
                scores.push_back(result);
            }

            other_scores.clear();
        }

        combine_result_t finish() final {
            return scores;
        }

    private:
        std::vector<score_t> scores;
    };

}

#endif //PROJECT_KEEP_ALL_COMBINER_H
