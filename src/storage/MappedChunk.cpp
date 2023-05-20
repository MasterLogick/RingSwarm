#include "MappedChunk.h"
#include <sys/mman.h>

namespace RingSwarm::storage {
    MappedChunk::~MappedChunk() {
#ifdef TGT_LINUX
        munmap(data, len);
#endif
#ifdef TGT_WINDOWS
#endif
#ifdef TGT_APPLE
#endif
    }
}