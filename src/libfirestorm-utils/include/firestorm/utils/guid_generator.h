//
// Created by sunside on 25.03.18.
//

#ifndef PROJECT_GUID_GENERATOR_H
#define PROJECT_GUID_GENERATOR_H

#include <memory>
#include "guid.h"

namespace firestorm {

    class guid_generator {
    public:
        static guid_generator& get_default() noexcept;

        guid_generator() noexcept;
        ~guid_generator() noexcept;

        guid create() const noexcept;

    private:
        class Impl;
        std::unique_ptr<Impl> _impl;
    };

}

#endif //PROJECT_GUID_GENERATOR_H
