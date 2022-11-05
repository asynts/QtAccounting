#pragma once

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QPushButton>
#include <QDialog>
#include <QStyledItemDelegate>

#include "Models/BillModel.hpp"
#include "Widgets/TransactionEditorDialog.hpp"

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

            fill_QComboBox_with_enum<Models::BillModel::Status>(m_ui.m_status_QComboBox);

            m_ui.m_transactions_QTableView->setModel(bill_model);
            m_ui.m_transactions_QTableView->setItemDelegate(new TransactionItemDelegate(this));
            m_ui.m_transactions_QTableView->setEditTriggers(QTableView::EditTrigger::DoubleClicked);

            m_ui.m_status_QComboBox->setCurrentIndex(static_cast<int>(bill_model->status()));

            connect(m_ui.m_status_QComboBox, &QComboBox::currentIndexChanged,
                    this, &BillEditorDialog::slotStatusChanged);

            connect(m_ui.m_new_QPushButton, &QPushButton::clicked,
                    this, &BillEditorDialog::slotNewTransaction);
        }

    private slots:
        void slotStatusChanged() {
            m_bill_model->setStatus(static_cast<Models::BillModel::Status>(m_ui.m_status_QComboBox->currentIndex()));
        }

        void slotNewTransaction()
        {
            TransactionEditorDialog dialog{ m_bill_model, nullptr };
            dialog.exec();
        }

    private:
        Ui::BillEditorDialog m_ui;

        Models::BillModel*const m_bill_model;
    };
}
