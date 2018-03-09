//
// Created by sunside on 09.03.18.
//

#ifndef PROJECT_TIME_CONVERSION_H
#define PROJECT_TIME_CONVERSION_H

namespace firestorm {

    /// \brief Converts milliseconds to seconds.
    /// \tparam T The data type.
    /// \param ms The number of milliseconds.
    /// \return The number of seconds.
    template<typename T>
    inline T ms_to_s(const T ms) {
        static_assert(std::is_arithmetic<T>::value, "type must be arithmetic");
        return ms * static_cast<T>(1000);
    }

}

#endif //PROJECT_TIME_CONVERSION_H
