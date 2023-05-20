#ifndef RINGSWARM_MAPPEDCHUNK_H
#define RINGSWARM_MAPPEDCHUNK_H

#include <cstdint>

namespace RingSwarm::storage {
    class MappedChunk {
        uint64_t len;
        void *data;
    public:
//        MappedChunk();

        MappedChunk(void *ptr, uint64_t len) : data(ptr), len(len) {
        };

        void *getData() {
            return data;
        };

        uint64_t getSize() {
            return len;
        }

        ~MappedChunk();
    };
}

#endif //RINGSWARM_MAPPEDCHUNK_H
