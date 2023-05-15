#include "PublicKey.h"
#include "../crypto/HashCrypto.h"

namespace RingSwarm::core {

    Id *PublicKey::getId() {
        return crypto::hashData(this);
    }
}