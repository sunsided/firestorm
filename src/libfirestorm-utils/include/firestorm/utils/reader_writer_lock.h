//
// Created by sunside on 10.03.18.
//

#ifndef PROJECT_READER_WRITER_LOCK_H
#define PROJECT_READER_WRITER_LOCK_H

#include <memory>
#include <mutex>
#include <shared_mutex>
#include <utility>

namespace firestorm {

    struct reader_lock_view_t;
    struct writer_lock_view_t;

    using reader_lock_view_ptr = std::shared_ptr<reader_lock_view_t>;
    using writer_lock_view_ptr = std::shared_ptr<writer_lock_view_t>;

    /// \brief A reader-writer lock.
    struct reader_writer_lock : private std::enable_shared_from_this<reader_writer_lock> {
    public:
        reader_writer_lock() = default;

        /// \brief Takes a read lock. Multiple threads can have this lock at the same time.
        /// \return The read lock.
        inline std::shared_lock<std::shared_mutex> read_lock() const {
            return std::shared_lock<std::shared_mutex>(_mutex);
        };

        /// \brief Takes a write lock. Only a single thread can have this lock.
        /// \return The write lock.
        inline std::unique_lock<std::shared_mutex> write_lock() const {
            return std::unique_lock<std::shared_mutex>(_mutex);
        };

        /// \brief Creates a writer view of this lock.
        /// \return The view.
        reader_lock_view_ptr reader_view() const;

        /// \brief Creates a reader view of this lock.
        /// \return The view.
        writer_lock_view_ptr writer_view() const;

    private:
        mutable std::shared_mutex _mutex;
    };

    /// \brief The reader view of a reader-writer lock.
    struct reader_lock_view_t final {
    public:
        explicit reader_lock_view_t(std::shared_ptr<const reader_writer_lock> lock)
            : _lock{std::move(lock)} {}

        /// \brief Takes a read lock. Multiple threads can have this lock at the same time.
        /// \return The read lock.
        inline std::shared_lock<std::shared_mutex> lock() const {
            return _lock->read_lock();
        }

    private:
        const std::shared_ptr<const reader_writer_lock> _lock;
    };

    /// \brief The writer view of a reader-writer lock.
    struct writer_lock_view_t final {
    public:
        explicit writer_lock_view_t(std::shared_ptr<const reader_writer_lock> lock)
            : _lock{std::move(lock)} {}

        /// \brief Takes a write lock. Only a single thread can have this lock.
        /// \return The write lock.
        inline std::unique_lock<std::shared_mutex> lock() const {
            return _lock->write_lock();
        }

    private:
        const std::shared_ptr<const reader_writer_lock> _lock;
    };

}

#endif //PROJECT_READER_WRITER_LOCK_H
