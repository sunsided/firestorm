//
// Created by sunside on 21.03.18.
//

#include <firestorm/engine/auto_reset_event.h>

namespace firestorm {

    auto_reset_event::auto_reset_event(bool initial) noexcept
            : _flag(initial)
    { }
}