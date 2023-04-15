#ifndef RINGSWARM_BUFFER_H
#define RINGSWARM_BUFFER_H

#include <cstdint>
#include <memory>
#include <vector>
#include "../proto/ProtocolException.h"
#include "../core/Id.h"
#include "../core/FileMeta.h"
#include "../core/Node.h"
#include "../core/ChunkLink.h"

namespace RingSwarm::transport {
    class Buffer {
        uint8_t *data;
        uint32_t len;
        uint32_t offset;
    public:
        explicit Buffer(uint32_t len, uint32_t offset = 0);

        ~Buffer();

        uint32_t getWrittenSize() {
            return offset;
        }

        uint8_t *getData() {
            return data;
        }

        uint64_t readUint64();

        uint16_t readUint16();

        uint8_t readUint8();

        core::Id *readId();

        core::FileMeta *readFileMeta();

        core::Node *readNode();

        std::vector<core::Node *> readNodeList();

        core::ChunkLink *readChunkLink();

        void writeUint64(uint64_t val);

        void writeUint32(uint32_t val);

        void writeUint8(uint8_t val);

        void writeId(core::Id id);

        void writeFileMeta(core::FileMeta *meta);

        void writeNode(std::shared_ptr<core::Node> node);

        void writeData(const char *data, uint32_t len);

        void writeChunkLink(core::ChunkLink &link);

        void writeNodeList(std::vector<std::shared_ptr<core::Node>> nodeList);
    };
}

#endif //RINGSWARM_BUFFER_H
