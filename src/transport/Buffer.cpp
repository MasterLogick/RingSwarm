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
        delete[] data;
    }

    template<>
    core::Id *Buffer::read<core::Id *>() {
        if (len - offset < 32) {
            throw DataSerialisationException();
        }
        auto *d = new core::Id();
        std::memcpy(d->hash, data + offset, 32);
        offset += 32;
        return d;
    }

    template<>
    void Buffer::write(core::Id *id) {
        if (len - offset < 32) {
            throw DataSerialisationException();
        }
        std::memcpy(data + offset, &id->hash, 32);
        offset += 32;
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

    template<>
    crypto::Signature *Buffer::read<crypto::Signature *>() {
        auto *sign = new crypto::Signature();
        readData(reinterpret_cast<char *>(sign->data()), sign->size());
        return sign;
    }

    template<>
    void Buffer::write(crypto::Signature *sign) {
        writeData(reinterpret_cast<char *>(sign->data()), sign->size());
    }

    template<>
    core::PublicKey *Buffer::read<core::PublicKey *>() {
        auto *key = new core::PublicKey();
        readData(reinterpret_cast<char *>(key->data()), key->size());
        return key;
    }

    template<>
    void Buffer::write(core::PublicKey *publicKey) {
        writeData(reinterpret_cast<char *>(publicKey->data()), publicKey->size());
    }

    template<>
    core::Node *Buffer::read<core::Node *>() {
        auto *node = new core::Node();
        node->publicKey = read<core::PublicKey *>();
        node->connectionInfo = ConnectionInfo::parseConnectionInfo(*this);
        node->id = crypto::hashData(node->publicKey);
        return node;
    }

    template<>
    void Buffer::write(core::Node *node) {
        write(node->publicKey);
        node->connectionInfo->serialize(*this);
    }

    template<>
    core::ChunkLink *Buffer::read<core::ChunkLink *>() {
        auto *keyId = read<core::Id *>();
        auto chunkIndex = read<uint64_t>();
        auto *dataHash = read<core::Id *>();
        auto sign = read<crypto::Signature *>();
        return new core::ChunkLink(keyId, chunkIndex, dataHash, sign);
    }

    template<>
    void Buffer::write(core::ChunkLink *link) {
        write(link->keyId);
        write<uint64_t>(link->chunkIndex);
        write(link->dataHash);
        write(link->sign);
    }

    template<>
    std::string Buffer::read<std::string>() {
        auto size = read<uint32_t>();
        if (len - offset < size) {
            throw DataSerialisationException();
        }
        std::string retVal(reinterpret_cast<const char *>(data + offset), size);
        offset += size;
        return retVal;
    }

    template<>
    void Buffer::write(std::string &str) {
        if (len - offset < str.size() + 4) {
            throw DataSerialisationException();
        }
        write<uint32_t>(str.size());
        std::memcpy(data + offset, str.c_str(), str.size());
        offset += str.size();

    }

    std::vector<char> Buffer::toBlob() {
        return std::vector<char>(data, data + len);
    }

    uint32_t Buffer::calcSize(core::Node *n) {
        return calcSize((core::PublicKey *) nullptr) + n->connectionInfo->getSerializedSize();
    }

    Buffer::Buffer(const Buffer &buffer) : data(buffer.data), offset(buffer.offset), len(buffer.len) {
    }

    Buffer::Buffer(Buffer &&buffer) : data(buffer.data), offset(buffer.offset), len(buffer.len) {
        buffer.data = nullptr;
    }
}