#pragma once

#include <QObject>
#include <QBindable>
#include <QSortFilterProxyModel>

#include "Persistance/Database.hpp"
#include "Models/TransactionModel.hpp"
#include "Util.hpp"

namespace Accounting::Models
{
    class DatabaseModel;

    class BillProxyModel final : public QSortFilterProxyModel {
    public:
        enum class Status {
            Staged,
            PendingPayment,
            ConfirmedPaid,
        };
        Q_ENUM(Status);

    private:
        Q_OBJECT

        Q_PROPERTY(QString id READ id WRITE setId BINDABLE bindableId NOTIFY signalChanged);
        Q_PROPERTY(QDate date READ date WRITE setDate BINDABLE bindableDate NOTIFY signalChanged);
        Q_PROPERTY(Status status READ status WRITE setStatus BINDABLE bindableStatus NOTIFY signalChanged);

    public:
        explicit BillProxyModel(DatabaseModel *database_model, QObject *parent = nullptr);

        TransactionModel* createTransaction(
                QDate date,
                qreal amount,
                QString category,
                TransactionModel::Status status,
                bool is_pocket_money);

        QString id() const { return m_id.value(); }
        void setId(QString value) { m_id = value; }
        QBindable<QString> bindableId() { return QBindable<QString>(&m_id); }

        Status status() const { return m_status.value(); }
        void setStatus(Status value) { m_status = value; }
        QBindable<Status> bindableStatus() { return QBindable<Status>(&m_status); }

        QDate date() const { return m_date.value(); }
        void setDate(QDate value) { m_date = value; }
        QBindable<QDate> bindableDate() { return QBindable<QDate>(&m_date); }

        qreal totalAmount() const {
            qreal result = 0.0;
            for (int row = 0; row < rowCount(); ++row) {
                auto *transaction = reinterpret_cast<TransactionModel*>(index(row, 0).internalPointer());

                if (transaction->status() == TransactionModel::Status::Normal) {
                    result += transaction->amount();
                }
            }
            return result;
        }

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

        virtual bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override {
            auto sourceIndex = sourceModel()->index(sourceRow, 0, sourceParent);
            auto *transaction_model = reinterpret_cast<TransactionModel*>(sourceIndex.internalPointer());

            return transaction_model->parentBillId() == m_id;
        }

    signals:
        void signalChanged();

    private:
        using QSortFilterProxyModel::setSourceModel;

        DatabaseModel *m_database_model;
        qint64 m_creation_timestamp;

        Q_OBJECT_BINDABLE_PROPERTY(BillProxyModel, QString, m_id, &BillProxyModel::signalChanged);
        Q_OBJECT_BINDABLE_PROPERTY(BillProxyModel, QDate, m_date, &BillProxyModel::signalChanged);
        Q_OBJECT_BINDABLE_PROPERTY(BillProxyModel, Status, m_status, &BillProxyModel::signalChanged);
    };
}
