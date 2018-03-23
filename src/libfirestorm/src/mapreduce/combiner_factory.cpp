//
// Created by sunside on 23.03.18.
//

#include <firestorm/engine/mapreduce/combiner_factory.h>

using namespace std;

namespace firestorm {

    combine_result_t combiner_factory::combine(vector<combiner_ptr> items) const {
        auto aggregator = create();

        aggregator->begin();
        for (auto& it : items) {
            auto result = it->finish();
            aggregator->combine(result);
        }
        return aggregator->finish();
    }

}