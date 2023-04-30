#include "FileMeta.h"

namespace RingSwarm::core {
    int FileMeta::getSerializedSize() {
        return 0;
    }

    Id *FileMeta::getId() {
        return fileId;
    }
} // core