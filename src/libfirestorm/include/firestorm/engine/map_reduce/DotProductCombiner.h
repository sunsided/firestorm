//
// Created by Markus on 11.02.2018.
//

#ifndef FIRESTORM_DOTPRODUCTCOMBINER_H
#define FIRESTORM_DOTPRODUCTCOMBINER_H

#include <vector>
#include "ChunkCombiner.h"

namespace firestorm {

    class DotProductCombiner final : public ChunkCombiner {
    public:
        DotProductCombiner() = default;
        ~DotProductCombiner() final = default;

        void begin() final {
            scores.clear();
        }

        void combine(std::any&& other) final {
            auto other_scores = std::any_cast<std::vector<score_t>>(other);

            // TODO: Later, this will be a "best N" thing
            for (const auto &result : other_scores) {
                scores.push_back(result);
            }

            other_scores.clear();
        }

        std::any finish() final {
            return scores;
        }

    private:
        std::vector<score_t> scores;
    };

}

#endif //FIRESTORM_DOTPRODUCTCOMBINER_H
