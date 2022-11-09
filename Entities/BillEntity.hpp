#pragma once

#include <QObject>
#include <QBindable>

#include "Persistance/Database.hpp"
#include "Util.hpp"

namespace Accounting::Entities
{
    class BillEntity final : public QObject {
    public:
        enum class Status {
            Staged,
            PendingPayment,
            ConfirmedPaid,
        };
        Q_ENUM(Status);

    private:
        Q_OBJECT

        Q_PROPERTY(QString id READ id BINDABLE bindableId NOTIFY signalChanged);
        Q_PROPERTY(QDate date READ date WRITE setDate BINDABLE bindableDate NOTIFY signalChanged);
        Q_PROPERTY(Status status READ status WRITE setStatus BINDABLE bindableStatus NOTIFY signalChanged);

    public:
        explicit BillEntity(QObject *parent = nullptr)
            : QObject(parent)
            , m_creation_timestamp(QDateTime::currentMSecsSinceEpoch()) { }

        QString id() const { return m_id.value(); }
        QBindable<QString> bindableId() { return QBindable<QString>(&m_id); }

        Status status() const { return m_status.value(); }
        void setStatus(Status value) { m_status = value; }
        QBindable<Status> bindableStatus() { return QBindable<Status>(&m_status); }

        QDate date() const { return m_date.value(); }
        void setDate(QDate value) { m_date = value; }
        QBindable<QDate> bindableDate() { return QBindable<QDate>(&m_date); }

        // FIXME: 'totalAmount'

        void deserialize(const Persistance::Bill& value) {
            m_id = value.m_id;
            m_date = value.m_date;
            m_status = enum_type_from_string<Status>(value.m_status);
            m_creation_timestamp = value.m_creation_timestamp;
        }

        Persistance::Bill serialize() const {
            return Persistance::Bill{
                .m_id = id(),
                .m_date = date(),
                .m_status = enum_type_to_string(status()),
                .m_creation_timestamp = m_creation_timestamp,
            };
        }

    signals:
        void signalChanged();

    private:
        qint64 m_creation_timestamp;

        Q_OBJECT_BINDABLE_PROPERTY(BillEntity, QString, m_id, &BillEntity::signalChanged);
        Q_OBJECT_BINDABLE_PROPERTY(BillEntity, QDate, m_date, &BillEntity::signalChanged);
        Q_OBJECT_BINDABLE_PROPERTY(BillEntity, Status, m_status, &BillEntity::signalChanged);
    };
}
