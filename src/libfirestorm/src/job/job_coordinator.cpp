//
// Created by sunside on 11.03.18.
//

#include <memory>
#include <utility>
#include <firestorm/engine/job/job_coordinator.h>
#include <firestorm/engine/job/job_info_t.h>
#include <firestorm/engine/job/job_t.h>

using namespace std;

namespace firestorm {

    class job_coordinator::Impl {
    public:
        explicit Impl(worker_thread_coordinator_ptr wtc) noexcept
            :_wtc{std::move(wtc)}
        {}

        ~Impl() noexcept = default;

        future<any> query(const mapper_factory_ptr &mf, const reducer_factory_ptr &rf,
                                           const vector_ptr &query) noexcept;

    private:
        job_info_ptr create_job_info() const;

    private:
        // TODO: Keep list of all jobs (remote ones, too)
        worker_thread_coordinator_ptr _wtc;
    };

    job_coordinator::job_coordinator(worker_thread_coordinator_ptr wtc) noexcept
        : _impl{make_unique<job_coordinator::Impl>(wtc)}
    {}

    job_info_ptr job_coordinator::Impl::create_job_info() const {
        return make_shared<job_info_t>();
    }

    future<any> job_coordinator::query(const mapper_factory_ptr &mf, const reducer_factory_ptr &rf,
                                const vector_ptr &query) noexcept {
        return _impl->query(mf, rf, query);
    }

    future<any> job_coordinator::Impl::query(const mapper_factory_ptr &mf, const reducer_factory_ptr &rf,
                                       const vector_ptr &query) noexcept {

        auto info = create_job_info();
        job_t job { info, mf, rf, query };

        // TODO: Schedule onto workers
        // TODO: Currently, worker_thread_coordinator itself contains aggregation logic for a final promise.
        //       However, multiple _machines_ could return these results, so the actual instance concerned
        //       with aggregation should be this coordinator.
        //       Either that, or we treat the result of the worker coordinator as the _same_ we
        //       would expect from a remote machine. In this case, the job coordinator only handles
        //       global output.
        // TODO: Add deadline support.
        return future<any>{};
    }

}