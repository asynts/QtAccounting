#pragma once

#include <QAbstractItemModel>
#include <QTextTableFormat>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextTable>
#include <QTextDocumentWriter>
#include <QDir>
#include <QSettings>

#include "Models/TransactionModel.hpp"
#include "Models/AbstractTransactionListModel.hpp"
#include "Util.hpp"
#include "Persistance/Database.hpp"

namespace Accounting::Models
{
    class DatabaseModel;

    class BillModel final : public AbstractTransactionListModel {
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
        explicit BillModel(DatabaseModel *parent_database_model);
        explicit BillModel(QString id, QDate date, Status status, qint64 creation_timestamp, DatabaseModel *parent_database_model);

        QString id() const { return m_id.value(); }
        QBindable<QString> bindableId() { return QBindable<QString>(&m_id); }

        Status status() const { return m_status.value(); }
        void setStatus(Status value) { m_status = value; }
        QBindable<Status> bindableStatus() { return QBindable<Status>(&m_status); }

        QDate date() const { return m_date.value(); }
        void setDate(QDate value) { m_date = value; }
        QBindable<QDate> bindableDate() { return QBindable<QDate>(&m_date); }

        qreal totalAmount() const {
            qreal total = 0.0;
            for (auto *transaction_model : m_owned_transactions) {
                if (transaction_model->status() == TransactionModel::Status::Normal) {
                    total += transaction_model->amount();
                } else if (transaction_model->status() == TransactionModel::Status::AlreadyPaid) {
                    // Exclude from total.
                } else {
                    Q_UNREACHABLE();
                }
            }
            return total;
        }

        void exportTo(std::filesystem::path path);

        Persistance::Bill serialize() const {
            QList<Persistance::Transaction> serialized_transactions;
            for (auto *transacton_model : m_owned_transactions) {
                serialized_transactions.append(transacton_model->serialize());
            }

            return Persistance::Bill{
                .m_id = id(),
                .m_date = date(),
                .m_status = enum_type_to_string(status()),
                .m_transactions = serialized_transactions,
                .m_creation_timestamp = m_creation_timestamp,
            };
        }

        void deserialize(const Persistance::Bill& value);

    signals:
        void signalChanged();

    public slots:
        void createTransaction(QDate date, qreal amount, QString category, TransactionModel::Status status);
        void deleteTransaction(Accounting::Models::TransactionModel *transaction_model);
        void deleteMyself();

    protected:
        virtual const QList<TransactionModel*> transaction_models_internal() const override {
            return m_owned_transactions;
        }

    private:
        qint64 m_creation_timestamp;

        QList<TransactionModel*> m_owned_transactions;

        Q_OBJECT_BINDABLE_PROPERTY(BillModel, QString, m_id, &BillModel::signalChanged);
        Q_OBJECT_BINDABLE_PROPERTY(BillModel, QDate, m_date, &BillModel::signalChanged);
        Q_OBJECT_BINDABLE_PROPERTY(BillModel, Status, m_status, &BillModel::signalChanged);
    };
}

Q_DECLARE_METATYPE(Accounting::Models::BillModel::Status);
