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
    KeyHandler *createKeyHandler(core::Id *keyId) {
        auto *keySwarm = storage::getKeySwarm(keyId);
        if (keySwarm != nullptr) {
            BOOST_LOG_TRIVIAL(debug) << "File download: Local cache hit for " << keyId->getHexRepresentation();
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
        std::vector<std::shared_ptr<async::Future<void>>> futures;
        futures.resize(threadCount);
        proto::ClientHandler *possibleKeySwarmNodeClient = nullptr;
        core::PublicKey *publicKey = nullptr;
        std::condition_variable keyFoundCondVar;
        std::mutex lockerMutex;
        for (int i = 0; i < threadCount; ++i) {
            std::function<void(core::PublicKey *, core::Node *, bool)> f;
            f = [&hitFlag, &publicKey, &lockerMutex, &keyFoundCondVar, &f, keyId, i, &startingPoints, &possibleKeySwarmNodeClient](
                    core::PublicKey *key,
                    core::Node *node,
                    bool isKeyFound) {
                if (hitFlag.test()) return;
                if (isKeyFound) {
                    if (hitFlag.test_and_set()) {
                        return;
                    }
                    publicKey = key;
                    possibleKeySwarmNodeClient = startingPoints[i];
                    std::lock_guard<std::mutex> l(lockerMutex);
                    keyFoundCondVar.notify_one();
                } else {
                    startingPoints[i] = std::get<0>(core::getOrConnect(node)->await());
                    startingPoints[i]->getKey(keyId, i)->then(f);

                }
            };
            futures[i] = startingPoints[i]->getKey(keyId, i)->then(f);
        }
        BOOST_LOG_TRIVIAL(debug) << "Started " << keyId->getHexRepresentation() << " key search using "
                                 << threadCount << " threads";
        std::unique_lock<std::mutex> lock(lockerMutex);
        keyFoundCondVar.wait(lock, [&hitFlag] { return hitFlag.test(); });
        BOOST_LOG_TRIVIAL(debug) << "Found " << keyId->getHexRepresentation() << " key";
        return new ExternalKeyHandler(keyId, publicKey, possibleKeySwarmNodeClient);
    }
}