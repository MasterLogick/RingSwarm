#define FUSE_USE_VERSION 310

#include "FuseController.h"

#include "../Assert.h"
#include "../client/ExternalKeyHandler.h"
#include "../client/FileDownloader.h"
#include "../core/Thread.h"

#include "FuseException.h"

#include <openssl/rand.h>

#include <cstring>
#include <filesystem>
#include <fstream>
#include <fuse_lowlevel.h>
#include <poll.h>
#include <thread>

namespace RingSwarm::fuse {
std::mutex filesLock;
std::map<
    core::Id *,
    std::pair<fuse_ino_t, client::KeyHandler *>,
    core::Id::Comparator>
    files;
std::atomic_int64_t nextInode;
fuse_session *session;
std::ifstream file;
unsigned long fsid;

auto getFileByInode(fuse_ino_t inode) {
    return std::find_if(files.begin(), files.end(), [inode](auto &x) {
        return x.second.first == inode;
    });
}

void fillStats(
    client::KeyHandler *handler,
    fuse_ino_t inode,
    struct stat &stats
) {
    stats.st_ino = inode;
    stats.st_mode = S_IFREG | S_IRUSR;
    stats.st_nlink = 1;
    stats.st_size = handler->getDataSize();
    stats.st_uid = getuid();
    stats.st_gid = getgid();
    stats.st_blksize = 4096;
    stats.st_blocks = 111111111;
}

void init(void *, struct fuse_conn_info *config) {
    BOOST_LOG_TRIVIAL(debug) << "FUSE created";
}

void lookup(fuse_req_t req, fuse_ino_t parent, const char *name) {
    BOOST_LOG_TRIVIAL(debug) << "FUSE lookup request " << parent << " " << name;
    if (parent == FUSE_ROOT_ID) {
        std::string path(name);
        if (path.length() != 64) {
            if (fuse_reply_err(req, ENOENT) != 0) {
                BOOST_LOG_TRIVIAL(debug) << "FUSE fuse_reply_err err";
            }
        } else if (!std::all_of(path.cbegin(), path.cend(), [](auto c) {
                       return '0' <= c <= '9' || 'a' <= c <= 'f';
                   })) {
            if (fuse_reply_err(req, ENOENT) != 0) {
                BOOST_LOG_TRIVIAL(debug) << "FUSE fuse_reply_err err";
            }
        } else {
            auto *keyId = core::Id::fromHexRepresentation(path.c_str());
            std::lock_guard<std::mutex> l(filesLock);
            auto iter = files.find(keyId);
            if (iter != files.end()) {
                fuse_entry_param entry{};
                memset(&entry, 0, sizeof(fuse_entry_param));
                entry.ino = iter->second.first;
                entry.attr_timeout = 1.0;
                entry.entry_timeout = 1.0;
                fillStats(iter->second.second, entry.ino, entry.attr);
                if (fuse_reply_entry(req, &entry) != 0) {
                    BOOST_LOG_TRIVIAL(debug) << "FUSE fuse_reply_entry err";
                }
            } else {
                if (fuse_reply_err(req, ENOENT) != 0) {
                    BOOST_LOG_TRIVIAL(debug) << "FUSE fuse_reply_err err";
                }
            }
        }
    } else {
        if (fuse_reply_err(req, ENOENT) != 0) {
            BOOST_LOG_TRIVIAL(debug) << "FUSE fuse_reply_err err";
        }
    }
}

void destroy(void *) {
    BOOST_LOG_TRIVIAL(debug) << "FUSE destroyed";
}

void getattr(fuse_req_t req, fuse_ino_t inode, fuse_file_info *) {
    BOOST_LOG_TRIVIAL(debug) << "FUSE getattr " << inode;
    if (inode == FUSE_ROOT_ID) {
        struct stat s {};

        memset(&s, 0, sizeof(struct stat));
        s.st_gid = getgid();
        s.st_uid = getuid();
        s.st_mode = S_IFDIR | S_IRUSR | S_IXUSR;
        s.st_size = 0;
        s.st_nlink = 2;
        s.st_ino = inode;
        s.st_blksize = 4096;
        s.st_blocks = 1;
        if (fuse_reply_attr(req, &s, 1.0) != 0) {
            BOOST_LOG_TRIVIAL(debug) << "FUSE fuse_reply_attr err";
        }
    } else {
        struct stat s {};

        memset(&s, 0, sizeof(struct stat));
        auto iter = getFileByInode(inode);
        fillStats(iter->second.second, inode, s);
        if (fuse_reply_attr(req, &s, 1.0) != 0) {
            BOOST_LOG_TRIVIAL(debug) << "FUSE fuse_reply_attr err";
        }
    }
}

void open(fuse_req_t req, fuse_ino_t inode, struct fuse_file_info *info) {
    BOOST_LOG_TRIVIAL(debug) << "FUSE open " << inode;
    if (inode != FUSE_ROOT_ID) {
        if ((info->flags & O_ACCMODE) != O_RDONLY) {
            if (fuse_reply_err(req, EACCES) != 0) {
                BOOST_LOG_TRIVIAL(debug) << "FUSE fuse_reply_err err";
            }
        }
        auto iter = getFileByInode(inode);
        if (iter != files.end()) {
            info->fh = (uint64_t) iter->first;
            if (fuse_reply_open(req, info) != 0) {
                BOOST_LOG_TRIVIAL(debug) << "FUSE fuse_reply_open err";
            }
        } else {
            if (fuse_reply_err(req, ENOENT) != 0) {
                BOOST_LOG_TRIVIAL(debug) << "FUSE fuse_reply_err err";
            }
        }
    } else {
        if (fuse_reply_err(req, EINVAL) != 0) {
            BOOST_LOG_TRIVIAL(debug) << "FUSE fuse_reply_err err";
        }
    }
}

void readBufferedStep(
    client::KeyHandler *handler,
    fuse_req_t req,
    char *buf,
    uint32_t buffOffset,
    off_t fileOffset,
    uint32_t remainderSize
) {
    handler->readData(remainderSize, fileOffset)
        ->then([handler, req, buf, buffOffset, fileOffset, remainderSize](
                   void *data,
                   uint32_t len
               ) {
            memcpy(buf + buffOffset, data, std::min(len, remainderSize));
            if (len < remainderSize) {
                readBufferedStep(
                    handler,
                    req,
                    buf,
                    buffOffset + len,
                    fileOffset + len,
                    remainderSize - len
                );
            } else {
                fuse_reply_buf(req, buf, buffOffset + remainderSize);
                delete[] buf;
            }
        });
}

void read(
    fuse_req_t req,
    fuse_ino_t inode,
    size_t size,
    off_t offset,
    struct fuse_file_info *info
) {
    BOOST_LOG_TRIVIAL(debug)
        << "FUSE read " << inode << " " << size << " " << offset;
    auto *id = reinterpret_cast<core::Id *>(info->fh);
    auto &p = files[id];
    Assert(p.first == inode);
    p.second->readData(size, offset)
        ->then(
            [handler = p.second, req, offset, size](void *data, uint32_t len) {
                if (len < size) {
                    char *buf = new char[size];
                    memcpy(buf, data, len);
                    readBufferedStep(
                        handler,
                        req,
                        buf,
                        len,
                        offset + len,
                        size - len
                    );
                } else {
                    if (fuse_reply_buf(
                            req,
                            reinterpret_cast<const char *>(data),
                            size
                        ) != 0) {
                        BOOST_LOG_TRIVIAL(debug) << "FUSE fuse_reply_buf err";
                    }
                }
            }
        );
}

void opendir(fuse_req_t req, fuse_ino_t inode, fuse_file_info *info) {
    BOOST_LOG_TRIVIAL(debug) << "FUSE opendir " << inode;
    if (inode == FUSE_ROOT_ID) {
        info->cache_readdir = 0;
        if (fuse_reply_open(req, info) != 0) {
            BOOST_LOG_TRIVIAL(debug) << "FUSE fuse_reply_open err";
        }
    } else {
        if (fuse_reply_err(req, ENOENT) != 0) {
            BOOST_LOG_TRIVIAL(debug) << "FUSE fuse_reply_err err";
        }
    }
}

void readdir(
    fuse_req_t req,
    fuse_ino_t inode,
    size_t size,
    off_t offset,
    struct fuse_file_info *info
) {
    BOOST_LOG_TRIVIAL(debug)
        << "FUSE readdir " << inode << " " << size << " " << offset;
    Assert(inode == 1);
    size_t bufSize = 0;
    if (offset < FUSE_ROOT_ID) {
        bufSize += fuse_add_direntry(req, nullptr, 0, ".", nullptr, 0) +
                   fuse_add_direntry(req, nullptr, 0, "..", nullptr, 0);
        // todo check size
    }
    std::lock_guard<std::mutex> l(filesLock);

    for (const auto &item: files) {
        if (offset >= item.second.first) {
            continue;
        }
        size_t s = fuse_add_direntry(
            req,
            nullptr,
            0,
            item.first->getHexRepresentation().c_str(),
            nullptr,
            0
        );
        if (bufSize + s > size) {
            break;
        }
        bufSize += s;
    }
    char *data = new char[bufSize];
    size_t off = 0;
    if (offset < FUSE_ROOT_ID) {
        struct stat s {};

        s.st_ino = FUSE_ROOT_ID;
        off += fuse_add_direntry(
            req,
            data + off,
            bufSize - off,
            ".",
            &s,
            FUSE_ROOT_ID
        );
        off += fuse_add_direntry(
            req,
            data + off,
            bufSize - off,
            "..",
            &s,
            FUSE_ROOT_ID
        );
    }

    struct stat s {};

    memset(&s, 0, sizeof(struct stat));
    for (const auto &item: files) {
        if (offset >= item.second.first) {
            continue;
        }
        fillStats(item.second.second, item.second.first, s);
        off += fuse_add_direntry(
            req,
            data + off,
            bufSize - off,
            item.first->getHexRepresentation().c_str(),
            &s,
            item.second.first
        );
        if (off == bufSize) {
            break;
        }
    }
    if (fuse_reply_buf(req, data, bufSize) != 0) {
        BOOST_LOG_TRIVIAL(debug) << "FUSE fuse_reply_buf err";
    }
    delete[] data;
}

void releasedir(fuse_req_t req, fuse_ino_t inode, struct fuse_file_info *) {
    BOOST_LOG_TRIVIAL(debug) << "FUSE releasedir " << inode;
    Assert(inode == 1);
    if (fuse_reply_err(req, 0) != 0) {
        BOOST_LOG_TRIVIAL(debug) << "FUSE fuse_reply_err err";
    }
}

void getxattr(fuse_req_t req, fuse_ino_t inode, const char *name, size_t size) {
    BOOST_LOG_TRIVIAL(debug) << "FUSE getxattr " << inode << " " << name;
    if (fuse_reply_err(req, ENOTSUP) != 0) {
        BOOST_LOG_TRIVIAL(debug) << "FUSE fuse_reply_err err";
    }
}

void flush(fuse_req_t req, fuse_ino_t, struct fuse_file_info *) {
    if (fuse_reply_err(req, 0) != 0) {
        BOOST_LOG_TRIVIAL(debug) << "FUSE fuse_reply_err err";
    }
}

void poll(
    fuse_req_t req,
    fuse_ino_t,
    struct fuse_file_info *,
    struct fuse_pollhandle *ph
) {
    if (fuse_reply_poll(req, POLLIN) != 0) {
        BOOST_LOG_TRIVIAL(debug) << "FUSE fuse_reply_poll err";
    }
    if (ph != nullptr) {
        if (fuse_lowlevel_notify_poll(ph) != 0) {
            BOOST_LOG_TRIVIAL(debug) << "FUSE fuse_lowlevel_notify_poll err";
        }
        fuse_pollhandle_destroy(ph);
    }
}

void statfs(fuse_req_t req, fuse_ino_t inode) {
    struct statvfs stats {
        4096, 4096, 111111111, 0, 0, static_cast<fsfilcnt_t>(nextInode.load()),
            0, 0, fsid, ST_RDONLY, 255
    };

    if (fuse_reply_statfs(req, &stats) != 0) {
        BOOST_LOG_TRIVIAL(debug) << "FUSE fuse_reply_statfs err";
    }
}

void startFuse(std::string &mountPoint) {
    fuse_lowlevel_ops ops{
        .init = init,
        .destroy = destroy,
        .lookup = lookup,
        .forget = nullptr,
        .getattr = getattr,
        .setattr = nullptr,
        .readlink = nullptr,
        .mknod = nullptr,
        .mkdir = nullptr,
        .unlink = nullptr,
        .rmdir = nullptr,
        .symlink = nullptr,
        .rename = nullptr,
        .link = nullptr,
        .open = open,
        .read = read,
        .write = nullptr,
        .flush = flush,
        .release = nullptr,
        .fsync = nullptr,
        .opendir = opendir,
        .readdir = readdir,
        .releasedir = releasedir,
        .fsyncdir = nullptr,
        .statfs = statfs,
        .setxattr = nullptr,
        .getxattr = getxattr,
        .listxattr = nullptr,
        .removexattr = nullptr,
        .access = nullptr,
        .create = nullptr,
        .getlk = nullptr,
        .setlk = nullptr,
        .bmap = nullptr,
        .ioctl = nullptr,
        .poll = nullptr,
        .write_buf = nullptr,
        .retrieve_reply = nullptr,
        .forget_multi = nullptr,
        .flock = nullptr,
        .fallocate = nullptr,
        .readdirplus = nullptr,
        .copy_file_range = nullptr,
        .lseek = nullptr
    };
    nextInode.store(FUSE_ROOT_ID + 1);
    char *d[] = {
        strdup("RingSwarm"),
            /*strdup("-oallow_other") /*,
         strdup("-d")*/
    };
    fuse_args args = FUSE_ARGS_INIT(sizeof(d) / sizeof(d[0]), d);
    session = fuse_session_new(&args, &ops, sizeof(fuse_lowlevel_ops), nullptr);
    if (session == nullptr) {
        throw FuseException();
    }
    if (fuse_session_mount(session, mountPoint.c_str()) != 0) {
        fuse_session_destroy(session);
        throw FuseException();
    }
    if (RAND_bytes(reinterpret_cast<unsigned char *>(&fsid), sizeof(fsid)) !=
        1) {
        // todo throw exception
    }
    file = std::ifstream("/home/user/Videos/2023-06-07 15-11-20.mp4");
    std::thread([] {
        core::setThreadName("Fuse main");
        if (fuse_session_loop(session) != 0) {
            // todo
        }
        /*fuse_loop_config cfg{1, 10};
        if (fuse_session_loop_mt(session, &cfg) != 0) {

        }*/
    }).detach();
}

void mountRing(core::Id *keyId) {
    std::lock_guard<std::mutex> l(filesLock);
    files.emplace(
        keyId,
        std::make_pair(nextInode++, client::createKeyHandler(keyId))
    );
}

void stopFuse() {
    fuse_session_unmount(session);
    fuse_session_destroy(session);
}
}// namespace RingSwarm::fuse
