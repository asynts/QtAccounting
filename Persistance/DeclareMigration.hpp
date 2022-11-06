#pragma once

#include <QMap>

// Very careful, this must not change otherwise all migration scripts need to be updated.
// The exact implementation can change but most must stay intact.
// This file should only depend on Qt.

namespace Accounting::Persistance
{
    struct MigrationId {
        quint64 m_old_binary_version;
        quint64 m_new_binary_version;
    };

    class Migration {
    public:
        explicit Migration(MigrationId migration_id, void (*function)())
            : m_migration_id(migration_id)
            , m_function(function) { }

        void run() {
            m_function();
        }

    private:
        MigrationId m_migration_id;
        void (*m_function)();
    };

    inline auto& migrations() {
        static QMap<MigrationId, Migration> map;
        return map;
    }

    struct MigrationRegistration {
        MigrationRegistration(void (*function)(), quint64 old_binary_version, quint64 new_binary_version) {
            MigrationId migration_id{
                .m_old_binary_version = old_binary_version,
                .m_new_binary_version = new_binary_version,
            };

            migrations().insert(migration_id, Migration{ migration_id, function });
        }
    };
}

#define REGISTER_MIGRATION(function, old_binary_version, new_binary_version) \
    ::Accounting::Persistance::MigrationRegistration accounting_migration_registration_ # old_binary_version # new_binary_version( \
        function, \
        old_binary_version, \
        new_binary_version \
    );
