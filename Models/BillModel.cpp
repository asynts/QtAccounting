#include "Models/BillModel.hpp"
#include "Models/DatabaseModel.hpp"
#include "Persistance/S3.hpp"

namespace Accounting::Models
{
    BillModel::BillModel(DatabaseModel *parent_database_model)
        : QAbstractItemModel(parent_database_model)
        , m_creation_timestamp(QDateTime::currentMSecsSinceEpoch())
        , m_database_model(parent_database_model) { }

    BillModel::BillModel(QString id, QDate date, Status status, qint64 creation_timestamp, DatabaseModel *parent_database_model)
        : QAbstractItemModel(parent_database_model)
        , m_id(id)
        , m_date(date)
        , m_status(status)
        , m_creation_timestamp(creation_timestamp)
        , m_database_model(parent_database_model) { }

    void BillModel::createTransaction(QDate date, qreal amount, QString category, TransactionModel::Status status) {
        auto *transaction_model = new TransactionModel(m_database_model->new_id(), date, amount, category, status, QDateTime::currentMSecsSinceEpoch(), this);

        int row = m_transactions.size();

        beginInsertRows(QModelIndex(), row, row);
        m_transactions.append(transaction_model);
        endInsertRows();

        connect(transaction_model, &TransactionModel::signalChanged,
                this, [=, this]() {
                    emit dataChanged(index(row, 0), index(row, columnCount() - 1));
                });
    }

    void BillModel::exportTo(std::filesystem::path path) {
        QSettings settings;
        QTextDocument document;
        QTextCursor cursor(&document);

        QTextCharFormat normalCharFormat;

        QTextCharFormat boldCharFormat;
        QFont boldFont = boldCharFormat.font();
        boldFont.setBold(true);
        boldCharFormat.setFont(boldFont);

        QTextCharFormat strikeOutCharFormat;
        QFont strikeOutFont = strikeOutCharFormat.font();
        strikeOutFont.setStrikeOut(true);
        strikeOutCharFormat.setFont(strikeOutFont);

        constexpr int column_count = 4;

        QList<std::array<QString, column_count>> data_displayText;
        QList<std::array<QTextCharFormat, column_count>> data_charFormat;
        QList<int> indices_emptyRow;
        QList<int> indices_valueRow;

        auto append_empty_row = [&]() {
            indices_emptyRow.append(data_displayText.size());

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
        };

        auto append_value_row = [&](QString key, QString value) {
            indices_valueRow.append(data_displayText.size());

            data_displayText.append({
                key,
                value,
                "",
                ""
            });
            data_charFormat.append({
                boldCharFormat,
                normalCharFormat,
                normalCharFormat,
                normalCharFormat,
            });
        };

        append_value_row("date", date().toString("yyyy-MM-dd"));
        append_value_row("bill_id", id());
        append_empty_row();

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

        // Transactions.
        for (TransactionModel *transaction_model : m_transactions) {
            QTextCharFormat charFormat;

            if (transaction_model->status() == TransactionModel::Status::Normal) {
                charFormat = normalCharFormat;
            } else if (transaction_model->status() == TransactionModel::Status::AlreadyPaid) {
                charFormat = strikeOutCharFormat;
            } else {
                Q_UNREACHABLE();
            }

            data_displayText.append({
                transaction_model->id(),
                transaction_model->date().toString("yyyy-MM-dd"),
                transaction_model->category(),
                QString::number(transaction_model->amount(), 'f', 2),
            });
            data_charFormat.append({
                charFormat,
                charFormat,
                charFormat,
                charFormat,
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

        append_empty_row();
        append_value_row("receiver", settings.value("Receiver/Name").value<QString>());
        append_value_row("iban", settings.value("Receiver/IBAN").value<QString>());

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

        // Remove divider lines from empty rows.
        for (int index_emptyRow : indices_emptyRow) {
            table->mergeCells(index_emptyRow, 0, 1, 4);
        }

        // Allow value of key-value pairs to be larger.
        for (int index_valueRow : indices_valueRow) {
            table->mergeCells(index_valueRow, 1, 1, 3);
        }

        // Ensure that the directory exists.
        std::filesystem::create_directories(path.parent_path());

        // Write the file to disk.
        QTextDocumentWriter writer(QString::fromStdString(path), "odf");
        writer.write(&document);

        // Upload the file to S3.
        Persistance::upload_file_to_s3(path, fmt::format("/Bills/{}", path.filename().string()));
    }
}
