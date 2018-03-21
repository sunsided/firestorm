//
// Created by sunside on 21.03.18.
//

#ifndef PROJECT_FUTURE_SUPPORT_H
#define PROJECT_FUTURE_SUPPORT_H

#include <future>

namespace firestorm {

    template<typename R>
    inline bool is_ready(const std::future<R>& f) {
        return f.valid() && (f.wait_for(std::chrono::seconds(0)) == std::future_status::ready);
    }

}

#endif //PROJECT_FUTURE_SUPPORT_H
