#pragma once

#include <QAbstractItemModel>
#include <QTextTableFormat>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextTable>
#include <QTextDocumentWriter>
#include <QDir>

#include "Models/TransactionModel.hpp"
#include "Util.hpp"

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

    private:
        Q_OBJECT

        Q_PROPERTY(QString id READ id BINDABLE bindableId NOTIFY signalChanged);
        Q_PROPERTY(QDate date READ date WRITE setDate BINDABLE bindableDate NOTIFY signalChanged);
        Q_PROPERTY(Status status READ status WRITE setStatus BINDABLE bindableStatus NOTIFY signalChanged);

    public:
        explicit BillModel(QString id, QDate date, Status status, QObject *parent = nullptr)
            : QAbstractItemModel(parent)
            , m_id(id)
            , m_date(date)
            , m_status(status) { }

        QString id() const { return m_id.value(); }
        QBindable<QString> bindableId() { return QBindable<QString>(&m_id); }

        Status status() const { return m_status.value(); }
        void setStatus(Status value) { m_status = value; }
        QBindable<Status> bindableStatus() { return QBindable<Status>(&m_status); }

        QDate date() const { return m_date.value(); }
        void setDate(QDate value) { m_date = value; }
        QBindable<QDate> bindableDate() { return QBindable<QDate>(&m_date); }

        void exportTo(QString filepath) {
            qDebug() << "Exporting to" << filepath;

            QTextDocument document;
            QTextCursor cursor(&document);

            enum ColumnEnum {
                MinColumn = 0,

                ColumnId = 0,
                ColumnDate,
                ColumnCategory,
                ColumnAmount,

                MaxColumn,
            };

            QList<std::array<QString, MaxColumn>> data;

            data.append({
                "id",
                "date",
                "category",
                "amount",
            });

            for (TransactionModel *transaction_model : m_transactions) {
                data.append({
                    transaction_model->id(),
                    transaction_model->date().toString("yyyy-MM-dd"),
                    transaction_model->category(),
                    QString::number(transaction_model->amount(), 'f', 2),
                });
            }

            QTextTable *table = cursor.insertTable(data.size(), MaxColumn);

            for (int row_index = 0; row_index < data.size(); ++row_index) {
                for (int column_index = MinColumn; column_index < MaxColumn; ++column_index) {
                    auto cell = table->cellAt(row_index, column_index);
                    cell.firstCursorPosition().insertText(data[row_index][column_index]);
                }
            }

            {
                bool ok = QFileInfo(filepath).dir().mkpath(".");
                Q_ASSERT(ok);
            }

            QTextDocumentWriter writer(filepath, "odf");
            writer.write(&document);
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
        Q_OBJECT_BINDABLE_PROPERTY(BillModel, QDate, m_date, &BillModel::signalChanged);
        Q_OBJECT_BINDABLE_PROPERTY(BillModel, Status, m_status, &BillModel::signalChanged);
    };
}

Q_DECLARE_METATYPE(Accounting::Models::BillModel::Status);
