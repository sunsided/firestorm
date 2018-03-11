//
// Created by Markus on 27.05.2017.
//

#ifndef FIRESTORM_REDUCER_PTR_H
#define FIRESTORM_REDUCER_PTR_H

#include <memory>
#include "reducer_t.h"

namespace firestorm {

    /// \brief Pointer to a reducer.
    using reducer_ptr = std::shared_ptr<reducer_t>;

}

#endif //FIRESTORM_REDUCER_PTR_H
