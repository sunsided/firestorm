//
// Created by Markus on 11.02.2018.
//

#ifndef FIRESTORM_KEEP_ALL_REDUCER_H
#define FIRESTORM_KEEP_ALL_REDUCER_H

#include <any>
#include <vector>
#include <firestorm/engine/types/score_t.h>
#include "firestorm/engine/mapreduce/reducer_t.h"
#include "score_result_t.h"

namespace firestorm {

    class keep_all_reducer final : public reducer_t {
    public:
        keep_all_reducer() = default;
        ~keep_all_reducer() final = default;

        void begin() final {
            _scores.clear();
        }

        void reduce(const combine_result& other) final {
            auto other_scores = other->any_cast<std::vector<score_t>>();
            reduce(other_scores);
        }

        void reduce(const reduce_result& other) final {
            auto other_scores = other->any_cast<std::vector<score_t>>();
            reduce(other_scores);
        }

        reduce_result finish() final {
            return std::make_shared<score_result_t>(_scores);
        }

    private:
        void reduce(const std::vector<score_t>& other_scores) {
            for (const auto &result : other_scores) {
                _scores.push_back(result);
            }
        }

    private:
        std::vector<score_t> _scores{};
    };

}

#endif //FIRESTORM_KEEP_ALL_REDUCER_H
