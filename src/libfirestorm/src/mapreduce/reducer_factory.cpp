//
// Created by sunside on 06.03.18.
//

#include <firestorm/engine/mapreduce/reducer_factory.h>

using namespace std;

namespace firestorm {

    any reducer_factory::reduce(vector<shared_ptr<reducer_t>> items) const {
        auto aggregator = create();

        aggregator->begin();
        for (auto& it : items) {
            auto result = it->finish();
            aggregator->reduce(result);
        }
        return aggregator->finish();
    }

}