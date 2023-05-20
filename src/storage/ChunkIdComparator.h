#ifndef RINGSWARM_CHUNKIDCOMPARATOR_H
#define RINGSWARM_CHUNKIDCOMPARATOR_H

#include <utility>
#include "../core/Id.h"

namespace RingSwarm::storage {
    class ChunkIdComparator {
    public:
        bool
        operator()(const std::pair<core::Id *, uint64_t> &lhs, const std::pair<core::Id *, uint64_t> &rhs) const {
            if (*lhs.first == *rhs.first) {
                return lhs.second < rhs.second;
            } else {
                return core::Id::Comparator()(lhs.first, rhs.first);
            }
        }
    };
}

#endif //RINGSWARM_CHUNKIDCOMPARATOR_H
