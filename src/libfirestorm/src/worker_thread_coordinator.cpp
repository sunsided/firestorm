//
// Created by sunside on 04.03.18.
//

#include <thread>
#include <vector>
#include <firestorm/engine/worker_t.h>
#include <firestorm/engine/worker_thread_coordinator.h>
#include "worker_thread.h"

using namespace std;

namespace firestorm {

    class worker_thread_coordinator::Impl {
    public:
        explicit Impl() noexcept
            : _outbox{make_shared<worker_outbox_t>()}
        { }

        void add_worker(bool redistribute = true) {
            const auto lock = _management_lock.write_lock();

            // Set CPU affinity
            // TODO: Explicitly setting CPU affinity might actually hurt performance if the core is loaded otherwise
            // TODO: L2 cache is shared between cores .. chunk sizes might affect performance of other cores?
            // set_thread_affinity(log, t, fun);

            // "emplace_back might leak if vector cannot be extended" ... unlikely but not worth the hassle
            // https://stackoverflow.com/a/15784982/195651
            _workers.push_back(make_unique<worker_thread>(make_unique<worker_t>(), _outbox, _management_lock.reader_view()));
            if (redistribute){
                internal_redistribute_chunks();
            }
        }

        void assign_chunk(const weak_ptr<const mem_chunk_t> &chunk, bool redistribute = true) {
            const auto lock = _management_lock.write_lock();

            _chunks.push_back(chunk);
            if (redistribute){
                internal_redistribute_chunks();
            }
        }

        void redistribute_chunks() {
            const auto lock = _management_lock.write_lock();
            internal_redistribute_chunks();
        }

        size_t effective_worker_count() const {
            size_t count = 0;
            for (auto& worker : _workers) {
                if (worker->num_chunks() == 0) continue;
                ++count;
            }
            return count;
        }

        std::future<std::any> process([[maybe_unused]] const job_t& job) const {
            const auto mapper = job.mapper_factory()->create();

            // TODO: Register request in a dictionary (on job ID)

            for (auto& worker : _workers) {
                if (worker->num_chunks() == 0) continue;
                auto reducer = job.reducer_factory()->create();
                auto cmd = make_shared<worker_query_cmd_t>(job.info(), job.query(), mapper, reducer);
                worker->enqueue_command(cmd);
            }

            // TODO: Tap the outbox of each worker for results on the job ID

            // TODO: Reduce result (do that multithreaded?)


            promise<any> lol {};
            lol.set_value(vector<score_t>{});
            return lol.get_future();
        }

    private:
        /// \brief Redistributes the chunks across workers.
        /// \warning The caller needs to take a write lock.
        void internal_redistribute_chunks() {
            // Drop all chunks that are deleted.
            // There could be a race condition where chunks expire after this call.
            // however, in that case this method would be called again soon until this
            // vector is eventually consistent.
            _chunks.erase(
                    std::remove_if(_chunks.begin(), _chunks.end(),
                                   [](const weak_ptr<const mem_chunk_t>& ptr) { return ptr.expired(); }),
                    _chunks.end());

            // No need to sort and apply when there is no-one to apply to.
            // The same logic as above applies for newly added workers.
            const auto num_workers = _workers.size();
            if (num_workers == 0) return;

            // Unassign all chunks from all workers.
            for (size_t w = 0; w < num_workers; ++w) {
                auto& worker = _workers[w];
                worker->unassign_all_chunks();
            }

            // Early exit v2.
            const auto num_chunks = _chunks.size();
            if (num_chunks == 0) return;

            // Sort chunks according to memory address.
            sort(_chunks.begin(), _chunks.end(),
                 [](const weak_ptr<const mem_chunk_t>& a, const weak_ptr<const mem_chunk_t>& b) {
                     const auto a_ptr = a.lock().get();
                     const auto b_ptr = b.lock().get();
                     return reinterpret_cast<size_t>(a_ptr) < reinterpret_cast<size_t>(b_ptr);
                 }
            );

            // Distribute the chunks across the workers.
            const auto chunks_per_worker = num_chunks / num_workers;
            size_t assigned_chunks = 0;
            for (size_t w = 0; w < num_workers; ++w) {
                auto& worker = _workers[w];
                for (size_t c = 0; c < chunks_per_worker; ++c) {
                    auto& chunk = _chunks[c];
                    worker->assign_chunk(chunk);
                    ++assigned_chunks;
                }
            }

            // Assign remaining chunks to last worker.
            auto& last_worker = _workers[num_workers - 1];
            for (auto c = assigned_chunks; c < num_chunks; ++c) {
                last_worker->assign_chunk(_chunks[c]);
            }
        }

    private:
        reader_writer_lock _management_lock {};
        worker_outbox_ptr _outbox;
        vector<unique_ptr<worker_thread>> _workers;
        vector<weak_ptr<const mem_chunk_t>> _chunks {};
    };

    worker_thread_coordinator::worker_thread_coordinator(size_t worker_count) noexcept
            : impl{make_unique<Impl>()} {
        // TODO: Throw if stupid
        worker_count = worker_count > 0 ? worker_count : 1;
        for (size_t i = 0; i < worker_count; ++i) {
            impl->add_worker(false);
        }
        impl->redistribute_chunks();
    }

    worker_thread_coordinator::~worker_thread_coordinator() = default;

    worker_thread_coordinator::worker_thread_coordinator(worker_thread_coordinator &&) noexcept = default;

    worker_thread_coordinator &worker_thread_coordinator::operator=(worker_thread_coordinator &&) noexcept = default;

    void worker_thread_coordinator::assign_chunk(std::weak_ptr<const mem_chunk_t> chunk) const {
        impl->assign_chunk(chunk);
    }

    std::future<std::any> worker_thread_coordinator::process(const job_t& job) const {
        return impl->process(job);
    }

    size_t worker_thread_coordinator::effective_worker_count() const {
        return impl->effective_worker_count();
    }

}