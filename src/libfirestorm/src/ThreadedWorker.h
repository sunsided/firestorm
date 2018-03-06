//
// Created by sunside on 04.03.18.
//

#ifndef PROJECT_THREADEDWORKER_H
#define PROJECT_THREADEDWORKER_H

#include <chrono>
#include <memory>
#include <thread>
#include <blockingconcurrentqueue.h>
#include <firestorm/engine/Worker.h>
#include <firestorm/engine/types/mem_chunk_t.h>
#include "worker_cmd_t.h"

namespace firestorm {

    class ThreadedWorker final {
    public:
        explicit ThreadedWorker(std::unique_ptr<Worker> worker) noexcept
                : worker{std::move(worker)}, queue{}, thread{[this] { process(); }} {
            // TODO: Bring up a worker thread
            // TODO: Configure the input queue
            // TODO: present results via promise/future
        }

        ~ThreadedWorker() {
            shutdown();
        }

        inline size_t num_chunks() const { return worker->num_chunks(); }

        void assign_chunk(const std::weak_ptr<const mem_chunk_t> &chunk) {
            worker->assign_chunk(chunk);
        }

        void shutdown() {
            // TODO: Check what happens if this is called twice
            queue.enqueue(worker_cmd_t{worker_cmd_enum_t::STOP});
            thread.join();
        }

    private:
        /// Processes all messages until an exit message arrives.
        void process() {
            while (true) {
                worker_cmd_t command{worker_cmd_enum_t::IDLE};
                if (!queue.wait_dequeue_timed(command, std::chrono::milliseconds(500))) {
                    // TODO: do household stuff
                    continue;
                }

                if (!process_sync(command)) {
                    break;
                }
            }
        }

        /// Processes a single command synchronously.
        /// \param command The command to process.
        /// \return A value indicating whether processing should continue (if true) or stop (if false).
        bool process_sync(worker_cmd_t &command) {
            switch (command.type) {
                default:
                    // TODO: throw an exception, as this is a logic error
                case STOP:
                    return false;
                case IDLE:
                    // TODO: could implement a liveness check to ensure no thread has crashed
                    return true;
                case QUERY:
                    query_vector(command.visitor, command.reducer, command.vector);
                    return true;
            }
        }

        /// Processes a query vector.
        /// \param query The vector to process.
        void query_vector(const std::shared_ptr<mapper_t> &visitor, const std::shared_ptr<combiner_t> &reducer, const std::shared_ptr<const vector_t> &query) {
            const auto &w = *worker;
            auto results = w.create_result_buffer();

            w.accept(*visitor, *reducer, *query);

            // TODO: What do we do with the results? Who creates the buffer?
        }

    private:
        const std::unique_ptr<Worker> worker;
        moodycamel::BlockingConcurrentQueue<worker_cmd_t> queue;
        std::thread thread;
    };

}

#endif //PROJECT_THREADEDWORKER_H
