#include "Id.h"
#include <boost/algorithm/hex.hpp>
#include <cstring>

namespace RingSwarm::core {
bool Id::operator==(const Id &rhs) const {
    return memcmp(hash, rhs.hash, 32) == 0;
}

bool Id::operator!=(const Id &rhs) const {
    return !(rhs == *this);
}

char intToHexFix(char c) {
    // unsafe transform. suppose input is trusted and in lower case
    if (c <= 9) {
        return c + '0';
    } else {
        return c - 10 + 'a';
    }
}

std::string Id::getHexRepresentation() {
    char d[64];
    for (int i = 0; i < 32; ++i) {
        d[i * 2] = intToHexFix(hash[i] >> 4);
        d[i * 2 + 1] = intToHexFix(hash[i] & 0xf);
    }
    return {d, 64};
}

char hexCharToInt(char c) {
    // unsafe transform. suppose input is trusted and in lower case
    if (c <= '9') {
        return c - '0';
    } else {
        return c - 'a' + 10;
    }
}

std::shared_ptr<Id> Id::fromHexRepresentation(const char *hex) {
    std::shared_ptr<Id> id;
    boost::algorithm::unhex(hex, id->hash);
    return id;
}

std::shared_ptr<Id> Id::fromBlob(std::vector<char> blob) {
    std::shared_ptr<Id> id;
    memcpy(id->hash, blob.data(), 32);
    return id;
}

std::shared_ptr<Id> Id::fromBlob(std::vector<char> &blob) {
    std::shared_ptr<Id> id;
    memcpy(id->hash, blob.data(), 32);
    return id;
}

bool Id::operator<(const Id &rhs) const {
    return memcmp(hash, rhs.hash, 32) < 0;
}
}// namespace RingSwarm::core