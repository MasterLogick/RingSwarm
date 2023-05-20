#include "ChunkStorage.h"
#include "StorageException.h"
#include "KeyIndexedStorages.h"

#ifdef TGT_LINUX

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <map>
#include <memory>

#endif
#ifdef TGT_WINDOWS
#endif
#ifdef TGT_APPLE
#endif
namespace RingSwarm::storage {
    ChunkIndexedStorage<std::shared_ptr<MappedChunk>> mappedChunks;

    std::shared_ptr<MappedChunk> getMappedChunk(core::Id *keyId, uint64_t chunkIndex) {
        auto p = std::pair(keyId, chunkIndex);
        if (mappedChunks.contains(p)) {
            return mappedChunks[p];
        }
        std::string path = "./storage/" + keyId->getHexRepresentation() + "." + std::to_string(chunkIndex);
#ifdef TGT_LINUX
        int fd = open(path.c_str(), O_RDONLY);
        if (fd == -1) {
            throw StorageException();
        }
        auto size = lseek(fd, 0, SEEK_END);
        void *mapping = mmap(nullptr, size, PROT_READ, MAP_SHARED, fd, 0);
        if (mapping == MAP_FAILED) {
            throw StorageException();
        }
        return (mappedChunks[p] = std::make_shared<MappedChunk>(mapping, size));
#endif
#ifdef TGT_WINDOWS
#endif
#ifdef TGT_APPLE
#endif
    }
}