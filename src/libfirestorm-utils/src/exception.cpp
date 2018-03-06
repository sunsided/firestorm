//
// Created by sunside on 24.02.18.
//

#include <iostream>

using namespace std;

namespace firestorm {

    #pragma clang diagnostic push
    #pragma ide diagnostic ignored "InfiniteRecursion"

    void print_exception(const std::exception &e, size_t level = 0) {
        cerr << std::string(level, ' ') << "exception: " << e.what() << '\n';
        try {
            std::rethrow_if_nested(e);
        } catch (const std::exception &e) {
            print_exception(e, level + 1);
        } catch (...) {
            // TODO: How to print this?
        }
    }

    #pragma clang diagnostic pop

}