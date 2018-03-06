//
// Created by sunside on 24.02.18.
//

#ifndef PROJECT_EXCEPTION_H
#define PROJECT_EXCEPTION_H

#include <exception>

namespace firestorm {

    void print_exception(const std::exception& e, size_t level = 0);

}

#endif //PROJECT_BENCHMARK_H
