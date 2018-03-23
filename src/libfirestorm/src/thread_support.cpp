//
// Created by sunside on 06.03.18.
//

#if USE_PTHREADS
#include <pthread.h>
#endif

#include <firestorm/engine/thread_support.h>

using namespace std;

namespace firestorm {

    void set_thread_affinity(const logger_t& log, size_t t, thread &thread) {
#if USE_PTHREADS
        cpu_set_t set {};
        CPU_ZERO(&set); // NOLINT
        CPU_SET(t, &set);
        const auto rc = pthread_setaffinity_np(thread.native_handle(), sizeof(cpu_set_t), &set);
        if (rc != 0) {
            log->error("Error setting thread affinity (pthread_setaffinity_np: {}).", rc);
        }
#endif
    }

}