#include <thread>
#include "FileDownloader.h"
#include "../core/ConnectionManager.h"
#include "../core/Settings.h"
#include <boost/log/trivial.hpp>
#include <mutex>
#include <condition_variable>
#include "../storage/KeySwarmStorage.h"
#include "CachedKeyHandler.h"
#include "ExternalKeyHandler.h"

namespace RingSwarm::client {
    KeyHandler *getKeyHandler(core::Id *keyId) {
        auto *keySwarm = storage::getKeySwarm(keyId);
        if (keySwarm != nullptr) {
            BOOST_LOG_TRIVIAL(debug) << "Local cache hit for " << keyId->getHexRepresentation();
            return new CachedKeyHandler(keyId);
        }
        std::atomic_flag hitFlag;
        int threadCount = 8;
        auto threadCountSetting = core::getSetting("key search thread count");
        if (!threadCountSetting.empty()) {
            threadCount = std::stoi(threadCountSetting);
        }
        std::vector<proto::ClientHandler *> startingPoints;
        for (int i = 0; i < threadCount; ++i) {
            auto *client = core::getPossibleKeyHost(keyId, 0);
            for (const auto &item: startingPoints) {
                if (*client->getRemote() == *item->getRemote()) {
                    client = nullptr;
                    break;
                }
            }
            if (client != nullptr) {
                startingPoints.push_back(client);
            }
        }
        threadCount = startingPoints.size();
        std::thread pool[threadCount];
        proto::ClientHandler *possibleKeySwarmNodeClient = nullptr;
        core::PublicKey *key = nullptr;
        std::condition_variable keyFoundCondVar;
        std::mutex lockerMutex;
        for (int i = 0; i < threadCount; ++i) {
            pool[i] = std::thread(
                    [&hitFlag, i, &startingPoints, keyId, &possibleKeySwarmNodeClient, &key, &keyFoundCondVar] {
                        auto *client = startingPoints[i];
                        while (!hitFlag.test()) {
                            core::PublicKey *localKey = nullptr;
                            core::Node *node = nullptr;
                            if (client->getKey(keyId, i, &localKey, &node)) {
                                if (hitFlag.test_and_set())
                                    return;
                                key = localKey;
                                possibleKeySwarmNodeClient = client;
                                keyFoundCondVar.notify_one();
                                return;
                            } else {
                                client = core::getOrConnect(node);
                            }
                        }
                    });
        }
        BOOST_LOG_TRIVIAL(debug) << "Started " << keyId->getHexRepresentation() << " key search using "
                                 << threadCount << " threads";
        std::unique_lock<std::mutex> lock(lockerMutex);
        keyFoundCondVar.wait(lock, [&hitFlag] { return hitFlag.test(); });
        BOOST_LOG_TRIVIAL(debug) << "Found " << keyId->getHexRepresentation() << " key";
        return new ExternalKeyHandler(keyId, key, possibleKeySwarmNodeClient);
    }
}