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
#include "Persistance.hpp"

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

        // FIXME: Implement
        static QString status_to_string(Status);
        static Status status_from_string(QString);

    private:
        Q_OBJECT

        Q_PROPERTY(QString id READ id BINDABLE bindableId NOTIFY signalChanged);
        Q_PROPERTY(QDate date READ date WRITE setDate BINDABLE bindableDate NOTIFY signalChanged);
        Q_PROPERTY(Status status READ status WRITE setStatus BINDABLE bindableStatus NOTIFY signalChanged);

    public:
        explicit BillModel(QObject *parent = nullptr)
            : QAbstractItemModel(parent) { }

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

        qreal totalAmount() const {
            qreal total = 0.0;
            for (auto *transaction_model : m_transactions) {
                total += transaction_model->amount();
            }
            return total;
        }

        // FIXME: Implement this in '.cpp' file.
        // FIXME: Maybe even create a helper class for this?
        void exportTo(QString filepath) {
            qDebug() << "Exporting to" << filepath;

            QSettings settings;
            QTextDocument document;
            QTextCursor cursor(&document);

            QTextCharFormat normalCharFormat;

            QTextCharFormat boldCharFormat;
            QFont boldFont = boldCharFormat.font();
            boldFont.setBold(true);
            boldCharFormat.setFont(boldFont);

            // FIXME: Move this into a function.
            {
                constexpr int column_count = 4;

                // Prepare data to be written into table.
                QList<std::array<QString, column_count>> data_displayText;
                QList<std::array<QTextCharFormat, column_count>> data_charFormat;

                // Date.
                int date_row = data_displayText.size();
                data_displayText.append({
                    "date",
                    date().toString("yyyy-MM-dd"),
                    "",
                    ""
                });
                data_charFormat.append({
                    boldCharFormat,
                    normalCharFormat,
                    normalCharFormat,
                    normalCharFormat,
                });

                // Bill ID.
                int bill_id_row = data_displayText.size();
                data_displayText.append({
                    "bill_id",
                    id(),
                    "",
                    ""
                });
                data_charFormat.append({
                    boldCharFormat,
                    normalCharFormat,
                    normalCharFormat,
                    normalCharFormat,
                });

                // Empty row.
                data_displayText.append({
                    "",
                    "",
                    "",
                    ""
                });
                data_charFormat.append({
                    normalCharFormat,
                    normalCharFormat,
                    normalCharFormat,
                    normalCharFormat,
                });

                // Table headers.
                data_displayText.append({
                    "id",
                    "date",
                    "category",
                    "amount",
                });
                data_charFormat.append({
                    boldCharFormat,
                    boldCharFormat,
                    boldCharFormat,
                    boldCharFormat,
                });

                // Table.
                for (TransactionModel *transaction_model : m_transactions) {
                    data_displayText.append({
                        transaction_model->id(),
                        transaction_model->date().toString("yyyy-MM-dd"),
                        transaction_model->category(),
                        QString::number(transaction_model->amount(), 'f', 2),
                    });
                    data_charFormat.append({
                        normalCharFormat,
                        normalCharFormat,
                        normalCharFormat,
                        normalCharFormat,
                    });
                }

                // Total.
                data_displayText.append({
                    "",
                    "",
                    "Total",
                    QString::number(totalAmount(), 'f', 2),
                });
                data_charFormat.append({
                    normalCharFormat,
                    normalCharFormat,
                    boldCharFormat,
                    boldCharFormat,
                });

                // Empty row.
                data_displayText.append({
                    "",
                    "",
                    "",
                    ""
                });
                data_charFormat.append({
                    normalCharFormat,
                    normalCharFormat,
                    normalCharFormat,
                    normalCharFormat,
                });

                // Receiver.
                const int receiver_row = data_displayText.size();
                data_displayText.append({
                    "Receiver",
                    settings.value("Receiver/Name").value<QString>(),
                    "",
                    ""
                });
                data_charFormat.append({
                    boldCharFormat,
                    normalCharFormat,
                    normalCharFormat,
                    normalCharFormat,
                });

                // IBAN
                const int iban_row = data_displayText.size();
                data_displayText.append({
                    "IBAN",
                    settings.value("Receiver/IBAN").value<QString>(),
                    "",
                    ""
                });
                data_charFormat.append({
                    boldCharFormat,
                    normalCharFormat,
                    normalCharFormat,
                    normalCharFormat,
                });

                // FIXME: Split this into a function and generate two tables with different borders.

                // Write to output file.
                Q_ASSERT(data_displayText.size() == data_charFormat.size());
                QTextTable *table = cursor.insertTable(data_displayText.size(), column_count);
                for (int row_index = 0; row_index < data_displayText.size(); ++row_index) {
                    for (int column_index = 0; column_index < column_count; ++column_index) {
                        auto cell = table->cellAt(row_index, column_index);

                        auto displayText = data_displayText[row_index][column_index];
                        auto charFormat = data_charFormat[row_index][column_index];

                        cell.firstCursorPosition().insertText(displayText, charFormat);
                    }
                }

                // The values won't fit otherwise.
                table->mergeCells(iban_row, 1, 1, 3);
                table->mergeCells(receiver_row, 1, 1, 3);
                table->mergeCells(bill_id_row, 1, 1, 3);
                table->mergeCells(date_row, 1, 1, 3);
            }

            {
                bool ok = QFileInfo(filepath).dir().mkpath(".");
                Q_ASSERT(ok);
            }

            QTextDocumentWriter writer(filepath, "odf");
            writer.write(&document);
        }

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
            };
        }

        void deserialize(const Persistance::Bill& value) {
            m_id = value.m_id;
            m_date = value.m_date;
            m_status = status_from_string(value.m_status);

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
            auto *transaction_model = new TransactionModel(generate_id(), date, amount, category, this);

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
        QList<TransactionModel*> m_transactions;

        Q_OBJECT_BINDABLE_PROPERTY(BillModel, QString, m_id, &BillModel::signalChanged);
        Q_OBJECT_BINDABLE_PROPERTY(BillModel, QDate, m_date, &BillModel::signalChanged);
        Q_OBJECT_BINDABLE_PROPERTY(BillModel, Status, m_status, &BillModel::signalChanged);
    };
}

Q_DECLARE_METATYPE(Accounting::Models::BillModel::Status);
