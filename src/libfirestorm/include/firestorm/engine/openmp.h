//
// Created by Markus on 24.02.2018.
//

#ifndef FIRESTORM_OPENMP_H
#define FIRESTORM_OPENMP_H

#include <string>
#include <sstream>

namespace firestorm {

#ifdef _OPENMP

    static inline bool openmp_available() noexcept { return true; }

#else
    static inline bool openmp_available() noexcept { return false; }
#endif

#ifdef USE_OPENMP

    static inline bool openmp_enabled() noexcept { return true; }

#else
    static inline bool openmp_enabled() noexcept { return false; }
#endif

#if _OPENMP

    static inline std::string openmp_version() noexcept {
        std::unordered_map<unsigned, std::string> map{
                {200505, "2.5"},
                {200805, "3.0"},
                {201107, "3.1"},
                {201307, "4.0"},
                {201511, "4.5"}};
        if (map.count(_OPENMP)) {
            return map.at(_OPENMP);
        }

        std::stringstream ss;
        ss << _OPENMP;
        return ss.str();
    }

#else
    static inline std::string openmp_version() noexcept { return "none"; }
#endif

}

#endif //FIRESTORM_OPENMP_H
