#ifndef RINGSWARM_ASYMETRICALCRYPTO_H
#define RINGSWARM_ASYMETRICALCRYPTO_H

#include <vector>
#include "../core/Id.h"

namespace RingSwarm::crypto {
    std::vector<char> signData(std::vector<char> data);

    bool verifyData(std::vector<char> &data, std::vector<char> &sig, std::vector<char> &pubKey);

    void loadNodeKeys();
}

#endif //RINGSWARM_ASYMETRICALCRYPTO_H
