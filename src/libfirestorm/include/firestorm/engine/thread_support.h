//
// Created by sunside on 06.03.18.
//

#ifndef PROJECT_THREAD_SUPPORT_H
#define PROJECT_THREAD_SUPPORT_H

#include <thread>
#include <firestorm/logging/logger_t.h>

namespace firestorm {
    void set_thread_affinity(const logger_t& log, size_t t, std::thread &thread);
}

#endif //PROJECT_THREAD_SUPPORT_H
