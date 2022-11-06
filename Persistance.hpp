#pragma once

#include <QString>
#include <QDate>
#include <QDataStream>
#include <QStandardPaths>
#include <QDir>

namespace Accounting::Persistance
{
    constexpr quint64 MAGIC_NUMBER = 7250402524647310127;
    constexpr quint64 BINARY_VERSON = 1;

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

    inline QString save_to_disk(const Database& database) {
        auto dirpath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        dirpath.append(QDir::separator());
        dirpath.append("Database");

        auto filepath = dirpath;
        filepath.append(QDir::separator());
        filepath.append(QString::number(QDateTime::currentMSecsSinceEpoch()).rightJustified(16, '0'));
        filepath.append("_Database.bin");

        qDebug() << "Writting output file to" << filepath;

        {
            bool ok = QFileInfo(filepath).dir().mkpath(".");
            Q_ASSERT(ok);
        }

        QFile file(filepath);
        file.open(QIODeviceBase::WriteOnly);

        QDataStream stream(&file);
        stream << MAGIC_NUMBER;
        stream << BINARY_VERSON;
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
        Q_ASSERT(binary_version == BINARY_VERSON);

        Database database;
        stream >> database;

        Q_ASSERT(stream.atEnd());

        return database;
    }
}
