//
// Created by sunside on 10.03.18.
//

#include "../include/firestorm/utils/reader_writer_lock.h"

using namespace std;

namespace firestorm {

    reader_lock_view_ptr reader_writer_lock::reader_view() const {
        return make_shared<reader_lock_view_t>(*this);
    }

    writer_lock_view_ptr reader_writer_lock::writer_view() const {
        return make_shared<writer_lock_view_t>(*this);
    }

}