#pragma once

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QPushButton>
#include <QDialog>
#include <QStyledItemDelegate>
#include <QStandardPaths>
#include <QDir>
#include <QDesktopServices>

#include <fmt/format.h>

#include "Models/BillModel.hpp"
#include "Models/DatabaseModel.hpp"
#include "Widgets/TransactionEditorDialog.hpp"
#include "Persistance/S3.hpp"

#include "ui_BillEditorDialog.h"

namespace Accounting::Widgets
{
    class TransactionItemDelegate final : public QStyledItemDelegate {
        Q_OBJECT

    public:
        explicit TransactionItemDelegate(QObject *parent = nullptr)
            : QStyledItemDelegate(parent) { }

        virtual bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem& option, const QModelIndex& index) override {
            if (!model->checkIndex(index)) {
                return false;
            }

            // All mouse events are delivered here for some reason.
            // The name 'editorEvent' is misleading.
            if (event->type() != QEvent::MouseButtonDblClick) {
                return false;
            }

            auto *bill_model = qobject_cast<Models::BillModel*>(model);
            auto *transaction_model = reinterpret_cast<Models::TransactionModel*>(index.internalPointer());

            TransactionEditorDialog dialog(bill_model, transaction_model);
            dialog.exec();

            return true;
        }
    };

    class BillEditorDialog final : public QDialog {
        Q_OBJECT

    public:
        explicit BillEditorDialog(Models::BillModel *bill_model, QWidget *parent = nullptr)
            : QDialog(parent)
            , m_bill_model(bill_model)
        {
            m_ui.setupUi(this);

            setWindowTitle(QString("Edit Bill '%1'").arg(bill_model->id()));

            {
                m_ui.m_transactions_QTableView->setModel(bill_model);
                m_ui.m_transactions_QTableView->setItemDelegate(new TransactionItemDelegate(this));
                m_ui.m_transactions_QTableView->setEditTriggers(QTableView::EditTrigger::DoubleClicked);

                m_ui.m_transactions_QTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
                m_ui.m_transactions_QTableView->horizontalHeader()->setSectionResizeMode(Models::BillModel::Columns::ColumnCategory, QHeaderView::ResizeMode::Stretch);
            }

            {
                fill_QComboBox_with_enum<Models::BillModel::Status>(m_ui.m_status_QComboBox);
                m_ui.m_status_QComboBox->setCurrentIndex(static_cast<int>(bill_model->status()));

                connect(m_ui.m_status_QComboBox, &QComboBox::currentIndexChanged,
                        this, &BillEditorDialog::slotStatusChanged);
            }

            {
                m_ui.m_date_QDateEdit->setDate(bill_model->date());

                connect(m_ui.m_date_QDateEdit, &QDateEdit::editingFinished,
                        this, &BillEditorDialog::slotDateChanged);
            }

            {
                connect(m_ui.m_new_QPushButton, &QPushButton::clicked,
                        this, &BillEditorDialog::slotNewTransaction);
            }

            {
                connect(m_ui.m_export_QPushButton, &QPushButton::clicked,
                        this, &BillEditorDialog::slotExport);
            }

            {
                connect(m_ui.m_delete_QPushButton, &QPushButton::clicked,
                        this, &BillEditorDialog::slotDelete);
            }
        }

    private slots:
        void slotDelete() {
            int reply = QMessageBox::question(
                        this,
                        "Delete?",
                        QString::fromStdString(fmt::format("Are you sure that you want to delete bill '{}'?", m_bill_model->id().toStdString())),
                        QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No);

            if (reply == QMessageBox::StandardButton::Yes) {
                m_bill_model->deleteMyself();
                done(QDialog::DialogCode::Accepted);
            }
        }

        void slotStatusChanged() {
            m_bill_model->setStatus(static_cast<Models::BillModel::Status>(m_ui.m_status_QComboBox->currentIndex()));
        }

        void slotDateChanged() {
            m_bill_model->setDate(m_ui.m_date_QDateEdit->date());
        }

        void slotNewTransaction()
        {
            TransactionEditorDialog dialog{ m_bill_model, nullptr };
            dialog.exec();
        }

        void slotExport()
        {
            auto path = Persistance::generate_local_path("Bills", fmt::format("{}_Bill.odt", m_bill_model->id().toStdString()));
            m_bill_model->exportTo(path);
            QDesktopServices::openUrl(QUrl::fromLocalFile(QString::fromStdString(path)));
        }

    private:
        Ui::BillEditorDialog m_ui;

        Models::BillModel *m_bill_model;
    };
}
