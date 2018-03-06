//
// Created by Markus on 04.03.2018.
//

#ifndef FIRESTORM_SCORE_T_H
#define FIRESTORM_SCORE_T_H

#include <cmath>
#include <memory>
#include "vector_idx_t.h"

namespace firestorm {

    struct score_t {
        score_t() noexcept
                : _vector_idx{0}, _score{std::nanf("")} {}

        score_t(const vector_idx_t vector_idx, const float score) noexcept
                : _vector_idx{vector_idx}, _score{score} {}

        score_t(const score_t &other) noexcept
                : _vector_idx{other._vector_idx}, _score{other._score} {}

        score_t &operator=(const score_t &other) noexcept {
            _vector_idx = other._vector_idx;
            _score = other._score;
            return *this;
        }

        /// Determines whether this score is valid.
        /// \return false if invalid, true if initialized.
        inline bool valid() const { return !invalid(); }

        /// Determines whether this score is invalid.
        /// \return true if invalid, false if initialized.
        inline bool invalid() const { return std::isnan(_score); }

        /// Gets the vector index.
        /// \return The vector index.
        inline vector_idx_t vector_idx() const { return _vector_idx; }

        /// Gets the result score.
        /// \return The score.
        inline float score() const { return _score; }

        inline bool operator==(const score_t &b) const {
            return _vector_idx == b._vector_idx && _score == b._score;
        }

        inline bool operator!=(const score_t &b) const {
            return !(*this == b);
        }

        inline bool operator>(const score_t &b) const {
            return *this > b._score;
        }

        inline bool operator<(const score_t &b) const {
            return *this < b._score;
        }

        inline bool operator>=(const score_t &b) const {
            return *this >= b._score;
        }

        inline bool operator<=(const score_t &b) const {
            return *this <= b._score;
        }

        inline bool operator==(const float &b) const {
            return _score == b;
        }

        inline bool operator!=(const float &b) const {
            return _score != b;
        }

        inline bool operator>(const float &b) const {
            return _score > b;
        }

        inline bool operator<(const float &b) const {
            return _score < b;
        }

        inline bool operator>=(const float &b) const {
            return _score >= b;
        }

        inline bool operator<=(const float &b) const {
            return _score <= b;
        }

    private:
        vector_idx_t _vector_idx;
        float _score;
    };

}

#endif //FIRESTORM_SCORE_T_H
