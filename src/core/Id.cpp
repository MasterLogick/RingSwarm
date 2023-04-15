#include <cstring>
#include "Id.h"

namespace RingSwarm::core {
    bool Id::operator==(const Id &rhs) const {
        return memcmp(hash, rhs.hash, 32) == 0;
    }

    bool Id::operator!=(const Id &rhs) const {
        return !(rhs == *this);
    }
}