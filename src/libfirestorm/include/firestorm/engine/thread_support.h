//
// Created by sunside on 06.03.18.
//

#ifndef PROJECT_THREAD_SUPPORT_H
#define PROJECT_THREAD_SUPPORT_H

#include <thread>
#include <spdlog/spdlog.h>

namespace firestorm {
    void set_thread_affinity(const std::shared_ptr<spdlog::logger> &log, size_t t, std::thread &thread);
}

#endif //PROJECT_THREAD_SUPPORT_H
