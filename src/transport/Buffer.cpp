#include "Buffer.h"
#include "DataSerialisationException.h"
#include <cstring>
#include <iostream>
#include "../core/Node.h"
#include "../crypto/AsymmetricalCrypto.h"
#include "../crypto/HashCrypto.h"

namespace RingSwarm::transport {
    Buffer::Buffer(uint32_t len, uint32_t offset) {
        data = new uint8_t[len];
        this->len = len;
        this->offset = offset;
    }

    Buffer::Buffer(std::vector<char> initValue) {
        data = new uint8_t[initValue.size()];
        memcpy(data, initValue.data(), initValue.size());
        len = 0;
        offset = 0;
    }

    Buffer::~Buffer() {
        delete data;
    }

    uint64_t Buffer::readUint64() {
        if (len - offset < 8) {
            throw DataSerialisationException();
        }
        uint64_t retVal = *reinterpret_cast<uint64_t *>(data + offset);
        offset += 8;
        return retVal;
    }

    uint32_t Buffer::readUint32() {
        if (len - offset < 4) {
            throw DataSerialisationException();
        }
        uint16_t retVal = *reinterpret_cast<uint16_t *>(data + offset);
        offset += 4;
        return retVal;
    }

    uint16_t Buffer::readUint16() {
        if (len - offset < 2) {
            throw DataSerialisationException();
        }
        uint16_t retVal = *reinterpret_cast<uint16_t *>(data + offset);
        offset += 2;
        return retVal;
    }

    uint8_t Buffer::readUint8() {
        if (len - offset < 1) {
            throw DataSerialisationException();
        }
        uint8_t retVal = data[offset];
        offset += 1;
        return retVal;
    }

    core::Id *Buffer::readId() {
        if (len - offset < 32) {
            throw DataSerialisationException();
        }
        auto *d = new core::Id();
        std::memcpy(d->hash, data + offset, 32);
        offset += 32;
        return d;
    }

    void Buffer::writeId(core::Id *id) {
        if (len - offset < 32) {
            throw DataSerialisationException();
        }
        std::memcpy(data + offset, &id->hash, 32);
        offset += 32;
    }

    void Buffer::writeUint64(uint64_t val) {
        if (len - offset < 8) {
            throw DataSerialisationException();
        }
        std::memcpy(data + offset, &val, 8);
        offset += 8;
    }

    void Buffer::writeUint32(uint32_t val) {
        if (len - offset < 4) {
            throw DataSerialisationException();
        }
        std::memcpy(data + offset, &val, 4);
        offset += 4;
    }

    void Buffer::writeUint8(uint8_t val) {
        if (len - offset < 1) {
            throw DataSerialisationException();
        }
        data[offset] = val;
        offset += 1;
    }

    void Buffer::writeData(const char *data, uint32_t len) {
        if (this->len - offset < len) {
            throw DataSerialisationException();
        }
        std::memcpy(this->data + offset, data, len);
        offset += len;
    }

    void Buffer::readData(char *data, uint32_t len) {
        if (this->len - offset < len) {
            throw DataSerialisationException();
        }
        std::memcpy(data, this->data + offset, len);
        offset += len;
    }

    core::FileMeta *Buffer::readFileMeta() {
        return nullptr;
    }

    void Buffer::writeFileMeta(core::FileMeta *meta) {

    }

    core::Node *Buffer::readNode() {
        auto *node = new core::Node();
        node->publicKey.resize(RAW_NODE_PUBLIC_KEY_LENGTH);
        readData(node->publicKey.data(), RAW_NODE_PUBLIC_KEY_LENGTH);
        node->connectionInfo = ConnectionInfo::parseConnectionInfo(*this);
        node->id = crypto::hashData(reinterpret_cast<const uint8_t *>(node->publicKey.data()),
                                    RAW_NODE_PUBLIC_KEY_LENGTH);
        return node;
    }

    void Buffer::writeNode(core::Node *node) {
        writeData(node->publicKey.data(), RAW_NODE_PUBLIC_KEY_LENGTH);
        node->connectionInfo->serialize(*this);
    }

    core::ChunkLink *Buffer::readChunkLink() {
        return nullptr;
    }

    void Buffer::writeChunkLink(core::ChunkLink *link) {
        writeId(link->file);
        writeUint64(link->chunkIndex);
        writeId(link->dataHash);
        writeData(link->sign.data(), link->sign.size());
    }

    std::vector<core::Node *> Buffer::readNodeList() {
        return {};
    }

    void Buffer::writeNodeList(std::vector<core::Node *> nodeList) {

    }

    std::string Buffer::readString() {
        uint32_t size = readUint32();
        if (len - offset < size) {
            throw DataSerialisationException();
        }
        std::string retVal(reinterpret_cast<const char *>(data + offset), size);
        offset += size;
        return retVal;
    }

    void Buffer::writeString(std::string &str) {
        if (len - offset < str.size() + 4) {
            throw DataSerialisationException();
        }
        writeUint32(str.size());
        std::memcpy(data + offset, str.c_str(), str.size());
        offset += str.size();

    }

    std::vector<char> Buffer::toBlob() {
        return std::vector<char>(data, data + len);
    }
}