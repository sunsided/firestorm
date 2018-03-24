//
// Created by sunside on 04.03.18.
//

#ifndef PROJECT_WORKER_CMD_T_H
#define PROJECT_WORKER_CMD_T_H

#include <memory>
#include <firestorm/engine/types/vector_t.h>
#include <firestorm/engine/job/job_info_t.h>
#include <firestorm/engine/mapreduce/mapper_t.h>
#include <firestorm/engine/mapreduce/combiner_t.h>

namespace firestorm {

    /// A structure describing a query command and its parameters.
    struct worker_query_cmd_t final {
        worker_query_cmd_t(job_info_ptr info, vector_ptr vector, mapper_ptr mapper, combiner_ptr combiner) noexcept
                : _info{std::move(info)}, _vector{std::move(vector)}, _mapper{std::move(mapper)}, _combiner{std::move(combiner)}
        {}

        ~worker_query_cmd_t() = default;
        worker_query_cmd_t(const worker_query_cmd_t&) = delete;
        worker_query_cmd_t(worker_query_cmd_t&&) = delete;

        inline job_info_ptr info() const { return _info; }
        inline vector_ptr vector() const { return _vector; }
        inline mapper_ptr mapper() const { return _mapper; }
        inline combiner_ptr combiner() const { return _combiner; }

        inline std::promise<combine_result>& promise() const { return _promise; }

    private:

        /// The type of the command to execute.
        const job_info_ptr _info;

        /// \brief A vector to execute commands on, if it exists.
        const vector_ptr _vector;

        /// \brief The visitor to process the chunks with.
        const mapper_ptr _mapper;

        /// \brief The combiner to combine the results with.
        const combiner_ptr _combiner;

        mutable std::promise<combine_result> _promise {};
    };

    /// \brief Pointer to a worker command.
    using worker_query_cmd_ptr = std::shared_ptr<worker_query_cmd_t>;

}

#endif //PROJECT_WORKER_CMD_T_H
