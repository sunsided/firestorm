//
// Created by Markus on 11.02.2018.
//

#ifndef FIRESTORM_KEEP_ALL_REDUCER_H
#define FIRESTORM_KEEP_ALL_REDUCER_H

#include <any>
#include <vector>
#include <firestorm/engine/types/score_t.h>
#include <firestorm/engine/mapreduce/typed_reducer_t.h>
#include "score_result_t.h"

namespace firestorm {

    class keep_all_reducer final : public typed_reducer_t<score_result_t> {
    public:
        keep_all_reducer() = default;
        ~keep_all_reducer() final = default;

        inline void begin() final {
            _scores.clear();
        }

        inline void reduce(const score_result_t& other) final {
            for (const auto &result : other.get()) {
                _scores.push_back(result);
            }
        }

        inline reduce_result finish() final {
            return std::make_shared<score_result_t>(_scores);
        }

    private:
        std::vector<score_t> _scores{};
    };

}

#endif //FIRESTORM_KEEP_ALL_REDUCER_H
