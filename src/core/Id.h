#ifndef RINGSWARM_ID_H
#define RINGSWARM_ID_H

#include <cstdint>
#include <string>
#include <vector>
#include <cstring>

namespace RingSwarm::core {
    struct Id {
        uint8_t hash[32];

        bool operator==(const Id &rhs) const;

        bool operator!=(const Id &rhs) const;

        std::string getHexRepresentation();

        static Id *fromHexRepresentation(const char *hex);

        static Id *fromBlob(std::vector<char> blob);

        class Comparator {
        public:
            bool operator()(Id *const lhs, Id *const rhs) const {
                return memcmp(lhs->hash, rhs->hash, 32) < 0;
            }
        };
    };
}

#endif //RINGSWARM_ID_H
