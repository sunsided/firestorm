//
// Created by Markus on 04.03.2018.
//

#ifndef FIRESTORM_SCORE_T_H
#define FIRESTORM_SCORE_T_H

#include <cmath>
#include <memory>
#include <tao/operators.hpp>
#include "index_t.h"
#include "score_value_t.h"

namespace firestorm {

    struct score_t : tao::operators::totally_ordered<score_t>,
                     tao::operators::totally_ordered<score_t, score_value_t> {
        score_t() noexcept
                : _index{0, 0}, _score{std::nanf("")} {}

        score_t(const index_t index, const score_value_t score) noexcept
                : _index{index}, _score{score} {}

        score_t(const score_t &other) noexcept
                : _index{other._index}, _score{other._score} {}

        score_t &operator=(const score_t &other) noexcept {
            _index = other._index;
            _score = other._score;
            return *this;
        }

        /// Determines whether this score is valid.
        /// \return false if invalid, true if initialized.
        [[ maybe_unused ]]
        inline bool valid() const noexcept { return !invalid(); }

        /// Determines whether this score is invalid.
        /// \return true if invalid, false if initialized.
        [[ maybe_unused ]]
        inline bool invalid() const noexcept { return std::isnan(_score); }

        /// Gets the vector index.
        /// \return The vector index.
        inline index_t index() const noexcept { return _index; }

        /// Gets the result score.
        /// \return The score.
        inline float score() const noexcept { return _score; }

        inline bool operator==(const score_t& b) const noexcept {
            return _index == b._index && _score == b._score;
        }

        inline bool operator<(const score_t& b) const noexcept {
            return *this < b._score;
        }

        inline bool operator==(const score_value_t& b) const noexcept {
            return _score == b;
        }

        inline bool operator>(const score_value_t& b) const noexcept {
            return _score > b;
        }

        inline bool operator<(const score_value_t& b) const noexcept {
            return _score < b;
        }

    private:
        index_t _index;
        score_value_t _score;
    };

}

#endif //FIRESTORM_SCORE_T_H
