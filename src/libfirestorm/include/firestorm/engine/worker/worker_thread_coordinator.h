//
// Created by sunside on 04.03.18.
//

#ifndef PROJECT_WORKERCOORDINATOR_H
#define PROJECT_WORKERCOORDINATOR_H

#include <memory>
#include <future>
#include <firestorm/engine/job/job_t.h>
#include <firestorm/engine/job/job_completion_callback_t.h>

namespace firestorm {

    class worker_thread_coordinator final {
    public:
        explicit worker_thread_coordinator(size_t worker_count) noexcept;

        ~worker_thread_coordinator();

        worker_thread_coordinator(worker_thread_coordinator &&) noexcept;

        worker_thread_coordinator &operator=(worker_thread_coordinator &&) noexcept;

        /// \brief Registers a memory chunk for processing.
        /// \param chunk The chunk to register.
        void assign_chunk(std::weak_ptr<const mem_chunk_t> chunk) const;

        /// \brief Gets the number of effective workers.
        /// \return The number of workers with actual chunks assigned.
        size_t effective_worker_count() const;

        /// \brief Processes the specified job.
        /// \param job The job to run.
        /// \return The outcome of the job.
        std::future<job_result_t> process(const job_t& job) const;

        /// \brief Processes the specified job.
        /// \param job The job to run.
        /// \param callback The callback to set the result at.
        void process(const job_t& job, job_completion_callback_t&& callback) const;

    private:
        class Impl;

        std::unique_ptr<Impl> impl;
    };

    using worker_thread_coordinator_ptr = std::shared_ptr<worker_thread_coordinator>;

}

#endif //PROJECT_WORKERCOORDINATOR_H
