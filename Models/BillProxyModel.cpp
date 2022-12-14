#include <QSettings>
#include <QTextDocument>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextTable>
#include <QTextDocumentWriter>

#include "Models/BillProxyModel.hpp"
#include "Models/DatabaseModel.hpp"
#include "Persistance/S3.hpp"

namespace Accounting::Models
{
    BillProxyModel::BillProxyModel(DatabaseModel *database_model, QObject *parent)
        : QSortFilterProxyModel(parent)
        , m_creation_timestamp(QDateTime::currentMSecsSinceEpoch())
        , m_parent_database_model(database_model)
    {
        setSourceModel(m_parent_database_model->transactionListModel());
    }

    TransactionModel* BillProxyModel::createTransaction(
            QDate date,
            qreal amount,
            QString category,
            TransactionModel::Status status,
            bool is_pocket_money)
    {
        auto *transaction_model = new TransactionModel(m_parent_database_model, this);
        transaction_model->setId(m_parent_database_model->consume_next_id());
        transaction_model->setParentBillId(id());
        transaction_model->setDate(date);
        transaction_model->setAmount(amount);
        transaction_model->setCategory(category);
        transaction_model->setStatus(status);
        transaction_model->setIsPocketMoney(is_pocket_money);

        m_parent_database_model->transactionListModel()->appendTransaction(transaction_model);

        return transaction_model;
    }
    void BillProxyModel::deleteMyself() {
        m_parent_database_model->billListModel()->deleteBill(this);
    }

    std::future<ResultEnum> BillProxyModel::exportTo_async(std::filesystem::path path) {
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
        for (TransactionModel *transaction_model : transaction_models()) {
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
        {
            std::error_code error_code;
            std::filesystem::create_directories(path.parent_path(), error_code);

            // If an error occured, return a resolved promise.
            if (error_code) {
                std::promise<ResultEnum> promise;
                promise.set_value(ResultEnum::Failure);
                return promise.get_future();
            }
        }

        // Write the file to disk.
        QTextDocumentWriter writer(QString::fromStdString(path), "odf");
        writer.write(&document);

        // Upload the file to S3.
        return std::async(std::launch::async, [path] {
            auto upload_succeeded = Persistance::upload_file_to_s3_async(path, fmt::format("/Bills/{}", path.filename().string())).get();

            if (upload_succeeded) {
                return ResultEnum::Success;
            } else {
                return ResultEnum::Failure;
            }
        });
    }
}
