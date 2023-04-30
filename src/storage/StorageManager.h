#ifndef RINGSWARM_STORAGEMANAGER_H
#define RINGSWARM_STORAGEMANAGER_H

class sqlite3;

namespace RingSwarm::storage {

    extern ::sqlite3 *dbConnection;

    void loadStorage(const char* dbPath);

    void closeStorage();
}

#endif //RINGSWARM_STORAGEMANAGER_H
