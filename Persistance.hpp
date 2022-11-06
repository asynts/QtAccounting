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

    QDataStream& operator>>(QDataStream& in, Transaction& value) {
        in >> value.m_id
           >> value.m_date
           >> value.m_amount
           >> value.m_category;

        return in;
    }

    QDataStream& operator>>(QDataStream& in, Bill& value) {
        in >> value.m_id
           >> value.m_date
           >> value.m_status
           >> value.m_transactions;

        return in;
    }

    QDataStream& operator>>(QDataStream& in, Database& value) {
        in >> value.m_bills;

        return in;
    }

    QDataStream& operator<<(QDataStream& out, const Transaction& value) {
        out << value.m_id
            << value.m_date
            << value.m_amount
            << value.m_category;

        return out;
    }

    QDataStream& operator<<(QDataStream& out, const Bill& value) {
        out << value.m_id
            << value.m_date
            << value.m_status
            << value.m_transactions;

        return out;
    }

    QDataStream& operator<<(QDataStream& out, const Database& value) {
        out << value.m_bills;

        return out;
    }

    QString save_to_disk(const Database& database) {
        auto filepath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        filepath.append(QDir::separator());
        filepath.append("Database");
        filepath.append(QDir::separator());
        filepath.append(QString::number(QDateTime::currentMSecsSinceEpoch()).rightJustified(16, '0'));
        filepath.append("_Database.bin");

        qDebug() << "Writting output file to" << filepath;

        QFile file(filepath);
        file.open(QIODeviceBase::WriteOnly);

        QDataStream stream(&file);
        stream << MAGIC_NUMBER;
        stream << BINARY_VERSON;
        stream << database;

        file.close();

        return filepath;
    }

    Database load_from_disk(QString filepath) {
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
