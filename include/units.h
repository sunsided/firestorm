//
// Created by Markus on 27.05.2017.
//

#ifndef FIRESTORM_UNITS_H
#define FIRESTORM_UNITS_H

#include <memory>

using bytes_t = size_t;

inline constexpr bytes_t operator"" _B (size_t amount) {
    return bytes_t(amount);
}

inline constexpr bytes_t operator"" _KB (size_t amount) {
    return bytes_t(1024UL*amount);
}

inline constexpr bytes_t operator"" _MB (size_t amount) {
    return bytes_t(1024UL*1024UL*amount);
}

#endif //FIRESTORM_UNITS_H
