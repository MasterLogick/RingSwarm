#include <thread>
#include "FileDownloader.h"
#include "../core/ConnectionManager.h"
#include "../core/Settings.h"
#include <boost/log/trivial.hpp>
#include <mutex>
#include <condition_variable>

namespace RingSwarm::client {
    FileHandler *getFileHandler(core::Id *fileId) {
        std::atomic_flag hitFlag;
        int threadCount = 8;
        auto threadCountSetting = core::getSetting("file search thread count");
        if (!threadCountSetting.empty()) {
            threadCount = std::stoi(threadCountSetting);
        }
        std::vector<proto::ClientHandler *> startingPoints;
        for (int i = 0; i < threadCount; ++i) {
            auto *client = core::getPossibleFileMetaHost(fileId, 0);
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
        proto::ClientHandler *possibleMetaRingNodeClient = nullptr;
        core::FileMeta *meta = nullptr;
        std::condition_variable metaFoundCV;
        std::mutex lockerMutex;
        for (int i = 0; i < threadCount; ++i) {
            pool[i] = std::thread(
                    [&hitFlag, i, &startingPoints, fileId, &possibleMetaRingNodeClient, &meta, &metaFoundCV] {
                        auto *client = startingPoints[i];
                        while (!hitFlag.test()) {
                            core::FileMeta *localMeta = nullptr;
                            core::Node *node = nullptr;
                            if (client->getFileMeta(fileId, i, &localMeta, &node)) {
                                if (hitFlag.test_and_set())
                                    return;
                                meta = localMeta;
                                possibleMetaRingNodeClient = client;
                                metaFoundCV.notify_one();
                                return;
                            } else {
                                client = core::getOrConnect(node);
                            }
                        }
                    });
        }
        BOOST_LOG_TRIVIAL(debug) << "Started " << fileId->getHexRepresentation() << " file meta search using "
                                 << threadCount << " threads";
        std::unique_lock<std::mutex> lock(lockerMutex);
        metaFoundCV.wait(lock, [&hitFlag] { return hitFlag.test(); });
        BOOST_LOG_TRIVIAL(debug) << "Found " << fileId->getHexRepresentation() << " file meta";
        return new FileHandler(meta, possibleMetaRingNodeClient);
    }
}