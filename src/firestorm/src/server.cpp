//
// Created by sunside on 21.03.18.
//

#include <thread>
#include <firestorm/engine/executor/local_executor_t.h>
#include "server.h"

using namespace firestorm;
using namespace std;

const size_t default_worker_count = 8;

int run_server() {
    // The instance identifier uniquely identifies this process.
    auto instance_identifier = make_shared<instance_identifier_t>();

    // The worker thread coordinator is used for local (in-process) work distribution.
    auto worker_count = thread::hardware_concurrency();
    if (worker_count == 0) worker_count = default_worker_count;
    auto wtc = make_shared<worker_thread_coordinator>(worker_count);

    // The local executor handles the local work coordination.
    local_executor_t local_executor{instance_identifier, wtc};

    // TODO: local_executor.process()

    return 0;
}