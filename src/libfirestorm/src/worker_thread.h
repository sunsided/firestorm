//
// Created by sunside on 04.03.18.
//

#ifndef PROJECT_WORKER_THREAD_H
#define PROJECT_WORKER_THREAD_H

#include <chrono>
#include <memory>
#include <thread>
#include <utility>
#include <blockingconcurrentqueue.h>
#include <firestorm/engine/worker_t.h>
#include <firestorm/engine/types/mem_chunk_t.h>
#include <firestorm/utils/reader_writer_lock.h>
#include "worker_cmd_t.h"
#include "worker_result_t.h"

namespace firestorm {

    /// \brief The type of the worker inbox.
    using worker_inbox_t = moodycamel::BlockingConcurrentQueue<worker_query_cmd_ptr>;

    /// \brief The type of the worker outbox.
    using worker_outbox_t = moodycamel::BlockingConcurrentQueue<worker_result_ptr>;

    /// \brief The type defining the worker outbox.
    using worker_outbox_ptr = std::shared_ptr<worker_outbox_t>;

    /// \brief Mailboxed thread around a worker.
    class worker_thread final {
    public:
        explicit worker_thread(std::unique_ptr<worker_t> worker, worker_outbox_ptr outbox, reader_lock_view_ptr lock) noexcept
            : _lock{std::move(lock)},
              _inbox{}, _outbox{std::move(outbox)},
              _worker{std::move(worker)}, _thread{[this] { process_loop(); }} {
        }

        ~worker_thread() {
            shutdown();
        }

        /// \brief Signals shutdown for this worker.
        void shutdown() {
            _shutdown.store(true);
            _thread.join();
        }

        /// \brief Enqueues a command for the worker.
        /// \param command The command to enqueue.
        inline void enqueue_command(worker_query_cmd_ptr& command) {
            _inbox.enqueue(std::move(command));
        }

        inline size_t num_chunks() const { return _worker->num_chunks(); }

        /// \brief Assigns a memory chunk to the worker.
        ///
        /// \warning The caller is required to take the management writer lock before calling this.
        /// \param chunk The chunk to assign.
        inline void assign_chunk(const std::weak_ptr<const mem_chunk_t> &chunk) {
            _worker->assign_chunk(chunk);
        }

        /// \brief Unassigns all chunks from the worker.
        ///
        /// \warning The caller is required to take the management writer lock before calling this.
        inline void unassign_all_chunks() {
            _worker->unassign_all_chunks();
        }

        /// Unassigns a chunk of the manager from this worker.
        /// \return The index of the chunk that was unassigned.
        inline boost::optional<std::weak_ptr<const mem_chunk_t>> unassign_chunk() {
            return _worker->unassign_chunk();
        }

    private:
        /// Processes all messages until an exit message arrives.
        void process_loop() {
            while (!_shutdown) {
                worker_query_cmd_ptr command;
                if (!_inbox.wait_dequeue_timed(command, _dequeue_timeout)) {
                    continue;
                }

                query_vector_sync(*command);
            }
        }

        /// Processes a query vector.
        /// \param query The vector to process.
        void query_vector_sync(const worker_query_cmd_t &command) {
            const auto lock = _lock->lock();

            const auto mapper = command.mapper();
            const auto reducer = command.reducer();
            const auto query = command.vector();

            reducer->begin();
            _worker->accept(*mapper, *reducer, *query);
            auto reduce_result = reducer->finish();

            command.promise().set_value(reduce_result);
        }

    private:
        /// \brief Signals shutdown of the worker.
        volatile std::atomic<bool> _shutdown {};

        /// \brief The dequeue timeout to use.
        const std::chrono::milliseconds _dequeue_timeout {500};

        /// \brief The management lock.
        const reader_lock_view_ptr _lock;

        /// \brief The inbox used to obtain processing commands.
        worker_inbox_t _inbox;

        /// \brief The outbox used to register processed results in.
        worker_outbox_ptr _outbox;

        /// \brief The underlying worker used to process the chunks.
        const std::unique_ptr<worker_t> _worker;

        /// \brief The thread that runs the worker.
        std::thread _thread;
    };

}

#endif //PROJECT_WORKER_THREAD_H
