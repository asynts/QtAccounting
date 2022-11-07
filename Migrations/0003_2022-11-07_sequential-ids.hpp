#pragma once

#include <QString>
#include <QDate>
#include <QDataStream>
#include <QStandardPaths>
#include <QDir>

// This file is used for two purposes:
//
// -   Save/load database to/from disk.
//
// -   Serve as migration script.
//
//     The script is standalone and only depends on Qt.
//
// The former is obvious, the latter is not.
//
// Suppose, we want to add a new field to 'Transaction':
//
//  1. Copy this file into the 'Migrations' folder.
//
//  2. Update 'NewTransaction' and 'migrate(const Transaction&)'.
//
//  3. Increment 'ACCOUTNING_NEW_BINARY_VERSON'.
//
//  4. Update namespace to be 'Accounting::Migrations::From${OLD_BINARY_VERSION}To${NEW_BINARY_VERSION}'.
//
//  5. Include the new file from 'MainWindow.cpp'.
//
//  6. Add call to 'Accounting::Migrations::From${OLD_BINARY_VERSION}To${NEW_BINARY_VERSION}::load_migrate_save()'.
//
// Then, update this file in-place:
//
//  1. Update 'Transaction' and 'migrate(const Transaction&)'.
//
//  2. Increment 'ACCOUTNING_NEW_BINARY_VERSON' and 'ACCOUTNING_OLD_BINARY_VERSON'.

#define ACCOUTNING_OLD_BINARY_VERSION 3ull
#define ACCOUTNING_NEW_BINARY_VERSION 4ull

#define ACCOUNTING_MAGIC_NUMBER 7250402524647310127ull

namespace Accounting::Migrations::From3To4
{
    struct Transaction {
        QString m_id;
        QDate m_date;
        qreal m_amount;
        QString m_category;
        qint64 m_creation_timestamp;
        QString m_status;
    };

    struct Bill {
        QString m_id;
        QDate m_date;
        QString m_status;
        QList<Transaction> m_transactions;
        qint64 m_creation_timestamp;
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
            .m_creation_timestamp = transaction.m_creation_timestamp,
            .m_status = transaction.m_status,
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
            .m_creation_timestamp = bill.m_creation_timestamp,
        };
    }

    struct NewDatabase {
        QList<Bill> m_bills;
        quint64 m_next_id;
    };

    inline NewDatabase migrate(const Database& database) {
        QList<NewBill> bills;
        for (auto& bill : database.m_bills) {
            bills.append(migrate(bill));
        }

        return NewDatabase{
            .m_bills = bills,
            .m_next_id = 1,
        };
    }

    inline QDataStream& operator>>(QDataStream& in, Transaction& value) {
        in >> value.m_id
           >> value.m_date
           >> value.m_amount
           >> value.m_category
           >> value.m_creation_timestamp
           >> value.m_status;

        return in;
    }

    inline QDataStream& operator>>(QDataStream& in, Bill& value) {
        in >> value.m_id
           >> value.m_date
           >> value.m_status
           >> value.m_transactions
           >> value.m_creation_timestamp;

        return in;
    }

    inline QDataStream& operator>>(QDataStream& in, Database& value) {
        in >> value.m_bills;

        return in;
    }

    inline QDataStream& operator<<(QDataStream& out, const NewTransaction& value) {
        out << value.m_id
            << value.m_date
            << value.m_amount
            << value.m_category
            << value.m_creation_timestamp
            << value.m_status;

        return out;
    }

    inline QDataStream& operator<<(QDataStream& out, const NewBill& value) {
        out << value.m_id
            << value.m_date
            << value.m_status
            << value.m_transactions
            << value.m_creation_timestamp;

        return out;
    }

    inline QDataStream& operator<<(QDataStream& out, const NewDatabase& value) {
        out << value.m_bills
            << value.m_next_id;

        return out;
    }

    inline void write_to_disk(const NewDatabase& database, std::filesystem::path toPath) {
        std::filesystem::create_directories(toPath.parent_path());

        QFile file(toPath);
        file.open(QIODeviceBase::WriteOnly);

        QDataStream stream(&file);
        stream << ACCOUNTING_MAGIC_NUMBER;
        stream << ACCOUTNING_NEW_BINARY_VERSION;
        stream << database;

        file.close();
    }

    inline Database read_from_disk(std::filesystem::path fromPath) {
        QFile file(fromPath);
        file.open(QIODeviceBase::ReadOnly);

        QDataStream stream(&file);

        quint64 magic_number;
        stream >> magic_number;
        Q_ASSERT(magic_number == ACCOUNTING_MAGIC_NUMBER);

        quint64 binary_version;
        stream >> binary_version;
        Q_ASSERT(binary_version == ACCOUTNING_OLD_BINARY_VERSION);

        Database database;
        stream >> database;

        Q_ASSERT(stream.atEnd());

        return database;
    }

    inline void migrate_on_disk(std::filesystem::path fromPath, std::filesystem::path toPath) {
        auto database = read_from_disk(fromPath);
        write_to_disk(migrate(database), toPath);
    }
}

#undef ACCOUTNING_OLD_BINARY_VERSION
#undef ACCOUTNING_NEW_BINARY_VERSION
#undef ACCOUNTING_MAGIC_NUMBER
