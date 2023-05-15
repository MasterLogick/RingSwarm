#include "Buffer.h"
#include "DataSerialisationException.h"
#include <cstring>
#include <iostream>
#include "../core/Node.h"
#include "../crypto/AsymmetricalCrypto.h"
#include "../crypto/HashCrypto.h"

namespace RingSwarm::transport {

//    template uint8_t Buffer::read<uint8_t>();

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
    core::FileMeta *Buffer::read<core::FileMeta *>() {
        auto *fileId = read<core::Id *>();
        auto *author = read<core::Id *>();
        auto creationTimestamp = read<uint64_t>();
        auto chunksCount = read<uint64_t>();
        auto chunkSize = read<uint32_t>();
        auto minSwarmSize = read<uint8_t>();
        auto ringConnectivity = read<uint8_t>();
        auto sign = read<crypto::Signature *>();
        return new core::FileMeta(author, creationTimestamp, chunksCount, chunkSize, minSwarmSize,
                                  ringConnectivity, sign, fileId);
    }

    template<>
    void Buffer::write(core::FileMeta *meta) {
        write(meta->fileId);
        write(meta->author);
        write<uint64_t>(meta->creationTimestamp);
        write<uint64_t>(meta->chunksCount);
        write<uint32_t>(meta->chunkSize);
        write<uint8_t>(meta->minSwarmSize);
        write<uint8_t>(meta->ringConnectivity);
        write(meta->sign);
    }

    template<>
    crypto::PublicKey *Buffer::read<crypto::PublicKey *>() {
        auto *key = new crypto::PublicKey();
        readData(reinterpret_cast<char *>(key->data()), key->size());
        return key;
    }

    template<>
    void Buffer::write(crypto::PublicKey *publicKey) {
        writeData(reinterpret_cast<char *>(publicKey->data()), publicKey->size());
    }

    template<>
    core::Node *Buffer::read<core::Node *>() {
        auto *node = new core::Node();
        node->publicKey = read<crypto::PublicKey *>();
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
        auto *fileId = read<core::Id *>();
        auto chunkIndex = read<uint64_t>();
        auto *dataHash = read<core::Id *>();
        auto sign = read<crypto::Signature *>();
        return new core::ChunkLink(fileId, chunkIndex, dataHash, sign);
    }

    template<>
    void Buffer::write(core::ChunkLink *link) {
        write(link->file);
        write<uint64_t>(link->chunkIndex);
        write(link->dataHash);
        write(link->sign);
    }

    template<>
    std::vector<core::Node *> Buffer::read<std::vector<core::Node *>>() {
        return {};
    }

    template<>
    void Buffer::write(std::vector<core::Node *> &nodeList) {

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
}