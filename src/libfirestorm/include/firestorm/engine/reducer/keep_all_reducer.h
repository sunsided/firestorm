//
// Created by Markus on 11.02.2018.
//

#ifndef FIRESTORM_KEEP_ALL_REDUCER_H
#define FIRESTORM_KEEP_ALL_REDUCER_H

#include <vector>
#include "reducer_t.h"

namespace firestorm {

    class keep_all_reducer final : public reducer_t {
    public:
        keep_all_reducer() = default;
        ~keep_all_reducer() final = default;

        void begin() final {
            scores.clear();
        }

        void reduce(const map_result_t& other) final {
            auto other_scores = std::any_cast<std::vector<score_t>>(other);

            for (const auto &result : other_scores) {
                scores.push_back(result);
            }

            other_scores.clear();
        }

        reduce_result_t finish() final {
            return scores;
        }

    private:
        std::vector<score_t> scores;
    };

}

#endif //FIRESTORM_KEEP_ALL_REDUCER_H
