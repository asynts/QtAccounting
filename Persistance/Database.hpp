#pragma once

#include <QString>
#include <QDate>
#include <QDataStream>
#include <QStandardPaths>
#include <QDir>

#include "Persistance/DeclareMigration.hpp"

// This file is used for two purposes:
//
// -   Save/load database to/from disk.
//
// -   Serve as migration script.
//
// The former is obvious, the latter is not.
// Suppose, we want to add a new field to 'Transaction':
//
//  1. Copy this file into the 'Migrations' folder.
//
//  2. Update 'NewTransaction' and 'migrate(const Transaction&)'.
//
//  3. Increment 'NEW_BINARY_VERSION'.
//
//  4. Update namespace to be 'Accounting::Migrations::From${OLD_BINARY_VERSION}To${NEW_BINARY_VERSION}'.
//
//  5. Include the new file from 'main.cpp'.
//
// You can run the migration script using 'load_migrate_save'.
// The script is standalone and only depends on Qt.
//
// Then, update this file in-place:
//
//  1. Update 'Transaction' and 'migrate(const Transaction&)'.
//
//  2. Increment 'NEW_BINARY_VERSION' and 'OLD_BINARY_VERSION'.

#define ACCOUTNING_OLD_BINARY_VERSON 1
#define ACCOUTNING_NEW_BINARY_VERSON 1

namespace Accounting::Persistance
{
    constexpr quint64 MAGIC_NUMBER = 7250402524647310127;

    struct Transaction {
        QString m_id;
        QDate m_date;
        qreal m_amount;
        QString m_category;
    };

    struct Bill {
        QString m_id;
        QDate m_date;
        QString m_status;
        QList<Transaction> m_transactions;
    };

    struct Database {
        QList<Bill> m_bills;
    };

    using NewTransaction = Transaction;
    inline NewTransaction migrate(const Transaction& transaction) {
        return NewTransaction{
            .m_id = transaction.m_id,
            .m_date = transaction.m_date,
            .m_amount = transaction.m_amount,
            .m_category = transaction.m_category,
        };
    }

    using NewBill = Bill;
    inline NewBill migrate(const Bill& bill) {
        QList<NewTransaction> transactions;
        for (auto& transaction : bill.m_transactions) {
            transactions.append(migrate(transaction));
        }

        return NewBill{
            .m_id = bill.m_id,
            .m_date = bill.m_date,
            .m_status = bill.m_status,
            .m_transactions = transactions,
        };
    }

    using NewDatabase = Database;
    inline NewDatabase migrate(const Database& database) {
        QList<NewBill> bills;
        for (auto& bill : database.m_bills) {
            bills.append(migrate(bill));
        }

        return NewDatabase{
            .m_bills = bills,
        };
    }


    inline QDataStream& operator>>(QDataStream& in, Transaction& value) {
        in >> value.m_id
           >> value.m_date
           >> value.m_amount
           >> value.m_category;

        return in;
    }

    inline QDataStream& operator>>(QDataStream& in, Bill& value) {
        in >> value.m_id
           >> value.m_date
           >> value.m_status
           >> value.m_transactions;

        return in;
    }

    inline QDataStream& operator>>(QDataStream& in, Database& value) {
        in >> value.m_bills;

        return in;
    }

    inline QDataStream& operator<<(QDataStream& out, const Transaction& value) {
        out << value.m_id
            << value.m_date
            << value.m_amount
            << value.m_category;

        return out;
    }

    inline QDataStream& operator<<(QDataStream& out, const Bill& value) {
        out << value.m_id
            << value.m_date
            << value.m_status
            << value.m_transactions;

        return out;
    }

    inline QDataStream& operator<<(QDataStream& out, const Database& value) {
        out << value.m_bills;

        return out;
    }

    inline QString save_to_disk(const NewDatabase& database) {
        auto dirpath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        dirpath.append(QDir::separator());
        dirpath.append("Database");

        auto filepath = dirpath;
        filepath.append(QDir::separator());
        filepath.append(QString::number(QDateTime::currentMSecsSinceEpoch()).rightJustified(16, '0'));
        filepath.append("_Database.bin");

        {
            bool ok = QFileInfo(filepath).dir().mkpath(".");
            Q_ASSERT(ok);
        }

        QFile file(filepath);
        file.open(QIODeviceBase::WriteOnly);

        QDataStream stream(&file);
        stream << MAGIC_NUMBER;
        stream << ACCOUTNING_NEW_BINARY_VERSON;
        stream << database;

        file.close();

        // Create symbolic link to discover current version.
        {
            auto current_path = QDir(dirpath).filePath("Database.bin");

            // This may fail if this is the first time we are creating this file.
            QFile::remove(current_path);

            QFile::link(filepath, current_path);
        }

        return filepath;
    }

    inline void automatic_migration(quint64 binary_version) {
        // This should not be used from a migration script.
        Q_ASSERT(ACCOUTNING_OLD_BINARY_VERSON == ACCOUTNING_NEW_BINARY_VERSON);

        // We can only migrate forward.
        Q_ASSERT(binary_version <= ACCOUTNING_OLD_BINARY_VERSON);

        // Migrate to the next binary version.
        MigrationId migration_id{
            .m_old_binary_version = binary_version,
            .m_new_binary_version = binary_version + 1,
        };
        migrations()[migration_id].run();
    }

    inline std::optional<Database> load_from_disk() {
        auto filepath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        filepath.append(QDir::separator());
        filepath.append("Database");
        filepath.append(QDir::separator());
        filepath.append("Database.bin");

        if (!QFile::exists(filepath)) {
            return std::nullopt;
        }

        QFile file(filepath);
        file.open(QIODeviceBase::ReadOnly);

        QDataStream stream(&file);

        quint64 magic_number;
        stream >> magic_number;
        Q_ASSERT(magic_number == MAGIC_NUMBER);

        quint64 binary_version;
        stream >> binary_version;

        if (binary_version != ACCOUTNING_OLD_BINARY_VERSON) {
            file.close();
            automatic_migration(binary_version);
        }

        Database database;
        stream >> database;

        Q_ASSERT(stream.atEnd());

        return database;
    }

    inline void load_migrate_save() {
        auto database_opt = load_from_disk();
        Q_ASSERT(database_opt.has_value());
        save_to_disk(migrate(database_opt.value()));
    }

    REGISTER_MIGRATION(&load_migrate_save, ACCOUTNING_OLD_BINARY_VERSON, ACCOUTNING_NEW_BINARY_VERSON);
}

#undef ACCOUTNING_OLD_BINARY_VERSION
#undef ACCOUTNING_NEW_BINARY_VERSION
