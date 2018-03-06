//
// Created by sunside on 04.03.18.
//

#ifndef PROJECT_WORKER_CMD_ENUM_H
#define PROJECT_WORKER_CMD_ENUM_H

#include <memory>
#include <firestorm/engine/vector_t.h>

namespace firestorm {

/// Describes a command to be executed by a threaded worker.
    enum worker_cmd_enum_t {
        IDLE = 0,       ///< A no-op command.
        STOP,           ///< Indicates that processing should stop.
        QUERY,          ///< Indicates that a query vector should be processed.
    };

/// A structure containing commands and their parameters.
    struct worker_cmd_t final {
        explicit worker_cmd_t(worker_cmd_enum_t command) noexcept
                : type{command}, vector{nullptr}, visitor{nullptr} {}

        explicit worker_cmd_t(std::shared_ptr<vector_t> vector, std::shared_ptr<ChunkMapper> visitor) noexcept
                : type{worker_cmd_enum_t::QUERY}, vector{std::move(vector)}, visitor{visitor} {}

        worker_cmd_t(const worker_cmd_t &other) noexcept
                : type{other.type}, vector{other.vector}, visitor{other.visitor} {}

        worker_cmd_t(worker_cmd_t &&other) noexcept
                : type{other.type}, vector{other.vector}, visitor{other.visitor} {}

        ~worker_cmd_t() = default;

        worker_cmd_t &operator=(const worker_cmd_t &other) noexcept {
            type = other.type;
            vector = other.vector;
            visitor = other.visitor;
            return *this;
        }

        /// The type of the command to execute.
        worker_cmd_enum_t type;

        /// A vector to execute commands on, if it exists.
        std::shared_ptr<vector_t> vector;

        /// The visitor to process the chunks with.
        std::shared_ptr<ChunkMapper> visitor;
    };

}

#endif //PROJECT_WORKER_CMD_ENUM_H
