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
    class DatabaseModel;

    class BillModel final : public QAbstractItemModel {
    public:
        enum class Status {
            Staged,
            PendingPayment,
            ConfirmedPaid,
        };
        Q_ENUM(Status);

        enum Columns {
            ColumnDate,
            ColumnAmount,
            ColumnCategory,
            ColumnStatus,
            ColumnId,

            COLUMN_COUNT
        };
        Q_ENUM(Columns);

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
            for (auto *transaction_model : m_transactions) {
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
            for (auto *transacton_model : m_transactions) {
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

        void deserialize(const Persistance::Bill& value) {
            m_id = value.m_id;
            m_date = value.m_date;
            m_status = enum_type_from_string<Status>(value.m_status);
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
            return Columns::COLUMN_COUNT;
        }

        virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override {
            if (orientation == Qt::Orientation::Vertical) {
                return QAbstractItemModel::headerData(section, orientation, role);
            }

            if (role != Qt::DisplayRole) {
                return QVariant();
            }

            if (section == Columns::ColumnId) {
                return "Id";
            } else if (section == Columns::ColumnStatus) {
                return "Status";
            } else if (section == Columns::ColumnDate) {
                return "Date";
            } else if (section == Columns::ColumnAmount) {
                return "Amount";
            } else if (section == Columns::ColumnCategory) {
                return "Category";
            } else {
                 Q_UNREACHABLE();
            }
        }

        virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override {
            if (index.row() < 0 || index.row() > rowCount()) {
                return QVariant();
            }

            if (role != Qt::DisplayRole) {
                return QVariant();
            }

            auto *transaction = m_transactions[index.row()];

            if (index.column() == Columns::ColumnDate) {
                return transaction->date().toString("yyyy-MM-dd");
            } else if (index.column() == Columns::ColumnAmount) {
                return QString::number(transaction->amount(), 'f', 2);
            } else if (index.column() == Columns::ColumnCategory) {
                return transaction->category();
            } else if (index.column() == Columns::ColumnId) {
                return transaction->id();
            } else if (index.column() == Columns::ColumnStatus) {
                return enum_type_to_string(transaction->status());
            }

            return QVariant();
        }

    signals:
        void signalChanged();

    public slots:
        void createTransaction(QDate date, qreal amount, QString category, TransactionModel::Status status);
        void deleteTransaction(Accounting::Models::TransactionModel *transaction_model);
        void deleteMyself();

    private:
        qint64 m_creation_timestamp;
        DatabaseModel *m_database_model;

        QList<TransactionModel*> m_transactions;

        Q_OBJECT_BINDABLE_PROPERTY(BillModel, QString, m_id, &BillModel::signalChanged);
        Q_OBJECT_BINDABLE_PROPERTY(BillModel, QDate, m_date, &BillModel::signalChanged);
        Q_OBJECT_BINDABLE_PROPERTY(BillModel, Status, m_status, &BillModel::signalChanged);
    };
}

Q_DECLARE_METATYPE(Accounting::Models::BillModel::Status);
