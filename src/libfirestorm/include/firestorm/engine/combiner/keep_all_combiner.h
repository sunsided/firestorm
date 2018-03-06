//
// Created by Markus on 11.02.2018.
//

#ifndef FIRESTORM_DOTPRODUCTCOMBINER_H
#define FIRESTORM_DOTPRODUCTCOMBINER_H

#include <vector>
#include "combiner_t.h"

namespace firestorm {

    class keep_all_combiner final : public combiner_t {
    public:
        keep_all_combiner() = default;
        ~keep_all_combiner() final = default;

        void begin() final {
            scores.clear();
        }

        void combine(std::any&& other) final {
            auto other_scores = std::any_cast<std::vector<score_t>>(other);

            // TODO: Later this will be a "best N" thing
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
