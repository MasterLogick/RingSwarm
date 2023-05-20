#ifndef RINGSWARM_KEYINDEXEDSTORAGES_H
#define RINGSWARM_KEYINDEXEDSTORAGES_H

#include <map>
#include "../core/Id.h"
#include "ChunkIdComparator.h"

namespace RingSwarm::storage {
    template<class T>
    using KeyIndexedStorage = std::map<core::Id *, T, core::Id::Comparator>;

    template<class T>
    using ChunkIndexedStorage = std::map<std::pair<core::Id *, uint64_t>, T, ChunkIdComparator>;
}

#endif //RINGSWARM_KEYINDEXEDSTORAGES_H
