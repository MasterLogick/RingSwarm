#define FUSE_USE_VERSION 310

#include "FuseController.h"
#include <fuse3/fuse.h>
#include <iostream>
#include <cstring>
#include <thread>
#include <fuse3/fuse_lowlevel.h>
#include <boost/log/trivial.hpp>
#include <filesystem>
#include <condition_variable>
#include "../client/ExternalKeyHandler.h"
#include "../client/FileDownloader.h"
#include <mutex>

namespace RingSwarm::fuse {
    std::map<std::string, client::KeyHandler *> openedKeys;
    std::vector<core::Id *> mountedRings;

    std::condition_variable fuseInitialisedCondVar;
    std::mutex fuseInitialisedMutex;
    bool fuseInitialised = false;
    std::string cwdBeforeFuseStart;

    void *init(struct fuse_conn_info *connInfo, struct fuse_config *config) {
        BOOST_LOG_TRIVIAL(debug) << "FUSE created";
        config->direct_io = 1;
        std::filesystem::current_path(cwdBeforeFuseStart);
        fuseInitialised = true;
        fuseInitialisedCondVar.notify_one();
        return nullptr;
    }

    void destroy(void *) {
        BOOST_LOG_TRIVIAL(debug) << "FUSE destroyed";
    }

    int getattr(const char *c, struct stat *stats, struct fuse_file_info *fi) {
        BOOST_LOG_TRIVIAL(debug) << "FUSE getattr " << c;
        memset(stats, 0, sizeof(struct stat));
        std::string path(c);
        if (path == "/") {
            stats->st_mode = S_IFDIR | 0444;
            stats->st_nlink = 2;
            return 0;
        }
        path = path.substr(1);
        if (path.length() != 64) {
            return -EACCES;
        }
        if (!std::all_of(path.cbegin(), path.cend(), [](auto c) { return '0' <= c <= '9' || 'a' <= c <= 'f'; })) {
            return -EACCES;
        }
        auto *keyId = core::Id::fromHexRepresentation(path.c_str());
        if (std::any_of(mountedRings.begin(), mountedRings.end(),
                        [keyId](core::Id *id) { return *keyId == *id; })) {
            stats->st_mode = S_IFREG | S_IRUSR;
            stats->st_nlink = 1;
            stats->st_size = 0;
            stats->st_uid = getuid();
            stats->st_gid = getgid();
            return 0;
        }
        return -ENOENT;
    }

    int readdir(const char *path, void *buff, fuse_fill_dir_t filter, off_t offset, struct fuse_file_info *fi,
                enum fuse_readdir_flags flags) {
        BOOST_LOG_TRIVIAL(debug) << "FUSE readdir " << path;
        filter(buff, ".", nullptr, 0, {});
        filter(buff, "..", nullptr, 0, {});
        for (const auto &item: mountedRings) {
            struct stat b{};
            b.st_mode = S_IFREG | S_IRUSR;
            b.st_size = 0;
            b.st_nlink = 1;
            b.st_uid = getuid();
            b.st_gid = getgid();
            filter(buff, item->getHexRepresentation().c_str(), &b, 0, {});
        }
        return 0;
    }

    /* int access(const char *c, int mode) {
         BOOST_LOG_TRIVIAL(debug) << "FUSE access " << c;
         std::string path(c);
         if (path == "/") {
             if (mode & (F_OK | R_OK)) {
                 return 0;
             } else {
                 return -EACCES;
             }
         }
         path = path.substr(1);
         if (path.length() != 64) {
             return -ENOENT;
         }
         if (!std::all_of(path.cbegin(), path.cend(), [](auto c) { return '0' <= c <= '9' || 'a' <= c <= 'f'; })) {
             return -ENOENT;
         }
         auto *keyId = core::Id::fromHexRepresentation(path.c_str());
         auto *keySwarm = storage::getKeySwarm(keyId);
         if (keySwarm != nullptr && (mode & (F_OK | R_OK))) {
             return 0;
         }
         if (std::any_of(mountedRings.begin(), mountedRings.end(),
                         [keyId](core::Id *id) { return *keyId == *id; }) && ()) {

         }
         return -ENOENT;
     }
 */
    int open(const char *c, struct fuse_file_info *info) {
        BOOST_LOG_TRIVIAL(debug) << "FUSE open " << c;
        std::string path(c + 1);
        if (path.length() != 64) {
            return -ENOENT;
        }
        if (!std::all_of(path.cbegin(), path.cend(), [](auto c) { return '0' <= c <= '9' || 'a' <= c <= 'f'; })) {
            return -ENOENT;
        }
        auto *keyId = core::Id::fromHexRepresentation(path.c_str());
        if (std::any_of(mountedRings.begin(), mountedRings.end(),
                        [keyId](core::Id *id) { return *keyId == *id; })) {
            auto *handler = client::createKeyHandler(keyId);
            if (handler) {
                openedKeys["/" + path] = handler;
                info->direct_io = 1;
                info->fh = reinterpret_cast<uint64_t>(handler);
                return 0;
            }
            return -ENOENT;
        } else {
            return -ENOENT;
        }
    }

    int read(const char *c, char *buff, size_t len, off_t offset, struct fuse_file_info *fi) {
        BOOST_LOG_TRIVIAL(debug) << "FUSE read " << c;
        auto *handler = reinterpret_cast<client::KeyHandler *>(fi->fh);
        if (handler == nullptr) {
            return EOF;
        }
        return handler->readData(buff, len, offset);
    }

    int release(const char *c, struct fuse_file_info *fi) {
        std::string path(c);
        openedKeys.erase(openedKeys.find(path));
        auto *handler = reinterpret_cast<client::KeyHandler *>(fi->fh);
        delete handler;
        return 0;
    }

    void startFuse(std::string &mountPoint) {
        std::unique_lock<std::mutex> lock(fuseInitialisedMutex);
        std::thread([&mountPoint] {
            fuse_operations ops{.getattr=getattr, .open=open, .read=read, .release=release, .readdir=readdir, .init=init, .destroy=destroy/*, .access=access*/};
            char *d[] = {strdup("RingSwarm"),
                         strdup(mountPoint.c_str()),
                         strdup("-f"),
                         strdup("-oallow_other"),
                         strdup("-d")};
            cwdBeforeFuseStart = std::filesystem::current_path().string();
            int ret = fuse_main(5, d, &ops, nullptr);
            if (ret != 0) {
                std::cout << ret << std::endl;
            }
        }).detach();
        fuseInitialisedCondVar.wait(lock, [] {
            return fuseInitialised;
        });
    }

    void mountRing(core::Id *keyId) {
        mountedRings.push_back(keyId);
    }
}