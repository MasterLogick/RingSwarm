#ifndef RINGSWARM_ID_H
#define RINGSWARM_ID_H

#include <cstdint>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

namespace RingSwarm::core {
struct Id {
    uint8_t hash[32];

    bool operator==(const Id &rhs) const;

    bool operator!=(const Id &rhs) const;

    bool operator<(const Id &rhs) const;

    std::string getHexRepresentation();

    static std::shared_ptr<Id> fromHexRepresentation(const char *hex);

    static std::shared_ptr<Id> fromBlob(std::vector<char> blob);

    static std::shared_ptr<Id> fromBlob(std::vector<char> &blob);

    class Comparator {
    public:
        bool operator()(Id * const lhs, Id * const rhs) const {
            return memcmp(lhs->hash, rhs->hash, 32) < 0;
        }
    };

    static std::shared_ptr<core::Id> getBiggestId() {
        return fromHexRepresentation(
            "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
        );
    }
};
}// namespace RingSwarm::core

#endif// RINGSWARM_ID_H
