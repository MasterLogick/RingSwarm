#include "Settings.h"
#include "../storage/Statement.h"
#include "../storage/StorageManager.h"

namespace RingSwarm::core {
    const char *settingInsert =
            "insert into settings (name, value)\n"
            "values (:name, :value);";

    void setSetting(const char *name, const char *value) {
        storage::Statement settingInsertStatement(storage::dbConnection, settingInsert);
        settingInsertStatement.bindText(":name", name);
        settingInsertStatement.bindText(":value", value);
        settingInsertStatement.execute();
    }

    const char *settingSelect = "select value\n"
                                "from settings\n"
                                "where name = :name;";

    std::string getSetting(const char *name) {
        storage::Statement settingSelectStatement(storage::dbConnection, settingSelect);
        settingSelectStatement.bindText(":name", name);
        if (settingSelectStatement.nextRow()) {
            return settingSelectStatement.getText(0);
        } else {
            return "";
        }
    }
}