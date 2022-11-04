#pragma once

#include <QAbstractItemModel>

#include "Models/TransactionModel.hpp"
#include "Util.hpp"

namespace Accounting::Models
{
    // FIXME: Rename to 'TransactionListModel'.
    class BillModel final : public QAbstractItemModel {
        Q_OBJECT

    public:
        enum class Status {
            Staged,
            PendingPayment,
            ConfirmedPaid,
        };
        Q_ENUM(Status);

        explicit BillModel(QString id, Status status, QObject *parent = nullptr)
            : QAbstractItemModel(parent)
            , m_id(id)
            , m_status(status) { }

        QString id() const { return m_id.value(); }
        QBindable<QString> bindableId() { return QBindable<QString>(&m_id); }

        Status status() const { return m_status.value(); }
        void setStatus(Status value) { m_status = value; }
        QBindable<Status> bindableDate() { return QBindable<Status>(&m_status); }

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
                return transaction->date();
            }

            if (index.column() == 1) {
                if (transaction->amount() <= 0.00) {
                    return "EXPENSE";
                } else {
                    return "INCOME";
                }
            }

            if (index.column() == 2) {
                return std::abs(transaction->amount());
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
            auto *transaction_object = new TransactionModel(generate_id(), date, amount, category, this);

            int row = m_transactions.size();

            beginInsertRows(QModelIndex(), row, row);
            m_transactions.append(transaction_object);
            endInsertRows();

            connect(transaction_object, &TransactionModel::signalChanged,
                    this, [=, this]() {
                        emit dataChanged(index(row, 0), index(row, columnCount() - 1));
                    });
        }

    private:
        QList<TransactionModel*> m_transactions;

        Q_OBJECT_BINDABLE_PROPERTY(BillModel, QString, m_id, &BillModel::signalChanged);
        Q_OBJECT_BINDABLE_PROPERTY(BillModel, Status, m_status, &BillModel::signalChanged);
    };
}

Q_DECLARE_METATYPE(Accounting::Models::BillModel::Status);
