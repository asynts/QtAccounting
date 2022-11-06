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
#include "Util.hpp"
#include "Persistance/Database.hpp"

namespace Accounting::Models
{
    // FIXME: Rename to 'TransactionListModel'.
    class BillModel final : public QAbstractItemModel {
    public:
        enum class Status {
            Staged,
            PendingPayment,
            ConfirmedPaid,
        };
        Q_ENUM(Status);

        static Status status_from_string(QString key) {
            bool ok;
            auto value = QMetaEnum::fromType<Status>().keyToValue(key.toUtf8(), &ok);
            Q_ASSERT(ok);

            return static_cast<Status>(value);
        }
        static QString status_to_string(Status value) {
            return QMetaEnum::fromType<Status>().valueToKey(static_cast<int>(value));
        }

    private:
        Q_OBJECT

        Q_PROPERTY(QString id READ id BINDABLE bindableId NOTIFY signalChanged);
        Q_PROPERTY(QDate date READ date WRITE setDate BINDABLE bindableDate NOTIFY signalChanged);
        Q_PROPERTY(Status status READ status WRITE setStatus BINDABLE bindableStatus NOTIFY signalChanged);

    public:
        explicit BillModel(QObject *parent = nullptr)
            : QAbstractItemModel(parent)
            , m_creation_timestamp(QDateTime::currentMSecsSinceEpoch()) { }

        explicit BillModel(QString id, QDate date, Status status, qint64 creation_timestamp, QObject *parent = nullptr)
            : QAbstractItemModel(parent)
            , m_id(id)
            , m_date(date)
            , m_status(status)
            , m_creation_timestamp(creation_timestamp) { }

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
            for (auto *transaction_model : m_transactions) {
                total += transaction_model->amount();
            }
            return total;
        }

        void exportTo(QString filepath);

        Persistance::Bill serialize() const {
            QList<Persistance::Transaction> serialized_transactions;
            for (auto *transacton_model : m_transactions) {
                serialized_transactions.append(transacton_model->serialize());
            }

            return Persistance::Bill{
                .m_id = id(),
                .m_date = date(),
                .m_status = status_to_string(status()),
                .m_transactions = serialized_transactions,
                .m_creation_timestamp = m_creation_timestamp,
            };
        }

        void deserialize(const Persistance::Bill& value) {
            m_id = value.m_id;
            m_date = value.m_date;
            m_status = status_from_string(value.m_status);
            m_creation_timestamp = value.m_creation_timestamp;

            beginResetModel();

            for (auto& transaction_model : m_transactions) {
                transaction_model->deleteLater();
            }
            m_transactions.clear();

            for (auto& transaction : value.m_transactions) {
                auto *transaction_model = new TransactionModel(this);
                transaction_model->deserialize(transaction);
                m_transactions.append(transaction_model);
            }

            endResetModel();
        }

        virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override {
            if (row < 0 || row >= rowCount()) {
                return QModelIndex();
            }

            return createIndex(row, column, reinterpret_cast<void*>(m_transactions[row]));
        }

        virtual QModelIndex parent(const QModelIndex& index) const override {
            return QModelIndex();
        }

        virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override {
            return m_transactions.size();
        }

        virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override {
            // Date
            // Expense
            // Amount
            // Category
            // Id
            return 5;
        }

        virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override {
            if (index.row() < 0 || index.row() > rowCount()) {
                return QVariant();
            }

            if (role != Qt::DisplayRole) {
                return QVariant();
            }

            auto *transaction = m_transactions[index.row()];

            if (index.column() == 0) {
                return transaction->date().toString("yyyy-MM-dd");
            }

            if (index.column() == 1) {
                if (transaction->amount() <= 0.00) {
                    return "EXPENSE";
                } else {
                    return "INCOME";
                }
            }

            if (index.column() == 2) {
                return QString::number(std::abs(transaction->amount()), 'f', 2);
            }

            if (index.column() == 3) {
                return transaction->category();
            }

            if (index.column() == 4) {
                return transaction->id();
            }

            return QVariant();
        }

    signals:
        void signalChanged();

    public slots:
        void createTransaction(QDate date, qreal amount, QString category) {
            auto *transaction_model = new TransactionModel(generate_id(), date, amount, category, QDateTime::currentMSecsSinceEpoch(), this);

            int row = m_transactions.size();

            beginInsertRows(QModelIndex(), row, row);
            m_transactions.append(transaction_model);
            endInsertRows();

            connect(transaction_model, &TransactionModel::signalChanged,
                    this, [=, this]() {
                        emit dataChanged(index(row, 0), index(row, columnCount() - 1));
                    });
        }

    private:
        qint64 m_creation_timestamp;

        QList<TransactionModel*> m_transactions;

        Q_OBJECT_BINDABLE_PROPERTY(BillModel, QString, m_id, &BillModel::signalChanged);
        Q_OBJECT_BINDABLE_PROPERTY(BillModel, QDate, m_date, &BillModel::signalChanged);
        Q_OBJECT_BINDABLE_PROPERTY(BillModel, Status, m_status, &BillModel::signalChanged);
    };
}

Q_DECLARE_METATYPE(Accounting::Models::BillModel::Status);
