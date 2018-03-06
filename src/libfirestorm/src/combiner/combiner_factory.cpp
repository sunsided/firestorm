//
// Created by sunside on 06.03.18.
//

#include <firestorm/engine/combiner/combiner_factory.h>

using namespace std;

namespace firestorm {

    any combiner_factory::combine_all(vector<shared_ptr<combiner_t>> items) const {
        auto aggregator = create();

        aggregator->begin();
        for (auto& it : items) {
            aggregator->combine(it->finish());
        }
        return aggregator->finish();
    }

}