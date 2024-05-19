#include "Buffer.h"

#include "../core/Node.h"
#include "../crypto/HashCrypto.h"

#include "DataSerialisationException.h"

#include <cstring>
#include <iostream>

namespace RingSwarm::transport {

Buffer::Buffer(uint32_t len, uint32_t offset) {
    if (len > 0) {
        data = new uint8_t[len];
    } else {
        data = nullptr;
    }
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
std::shared_ptr<core::Id> Buffer::read<std::shared_ptr<core::Id>>() {
    if (len - offset < 32) {
        throw DataSerialisationException();
    }
    auto d = std::make_shared<core::Id>();
    std::memcpy(d->hash, data + offset, 32);
    offset += 32;
    return d;
}

template<>
void Buffer::write(std::shared_ptr<core::Id> id) {
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
std::shared_ptr<core::PublicKey>
Buffer::read<std::shared_ptr<core::PublicKey>>() {
    auto key = std::make_shared<core::PublicKey>();
    readData(reinterpret_cast<char *>(key->data()), key->size());
    return key;
}

template<>
void Buffer::write(std::shared_ptr<core::PublicKey> publicKey) {
    writeData(reinterpret_cast<char *>(publicKey->data()), publicKey->size());
}

template<>
core::Node *Buffer::read<core::Node *>() {
    auto *node = new core::Node();
    node->publicKey = read<std::shared_ptr<core::PublicKey>>();
    node->id = crypto::hashData(*node->publicKey);
    return node;
}

template<>
void Buffer::write(core::Node *node) {
    write(node->publicKey);
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
    return calcSize((core::PublicKey *) nullptr);
}

Buffer::Buffer(Buffer &&buffer)
    : data(buffer.data), offset(buffer.offset), len(buffer.len) {
    buffer.data = nullptr;
}

Buffer &Buffer::operator=(Buffer &&buffer) noexcept {
    data = buffer.data;
    offset = buffer.offset;
    len = buffer.len;
    buffer.data = nullptr;
    return *this;
}
}// namespace RingSwarm::transport
