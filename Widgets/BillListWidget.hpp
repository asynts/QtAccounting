#pragma once

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QPushButton>
#include <QStyledItemDelegate>
#include <QPainter>

#include "Models/DatabaseModel.hpp"
#include "Widgets/BillEditorDialog.hpp"
#include "Widgets/FutureProgressDialog.hpp"

#include "ui_BillListWidget.h"

namespace Accounting::Widgets
{
    class BillItemDelegate final : public QStyledItemDelegate {
        Q_OBJECT

    public:
        explicit BillItemDelegate(QObject *parent = nullptr)
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

            auto *bill_model = reinterpret_cast<Models::BillProxyModel*>(index.internalPointer());

            BillEditorDialog dialog(bill_model);
            dialog.exec();

            return true;
        }
    };

    class BillListWidget final : public QWidget {
        Q_OBJECT

    public:
        explicit BillListWidget(QWidget *parent = nullptr)
            : QWidget(parent)
        {
            m_ui.setupUi(this);

            connect(m_ui.m_new_QPushButton, &QPushButton::clicked,
                    this, &BillListWidget::slotNewBill);
        }

        void setModel(Models::DatabaseModel *database_model) {
            m_ui.m_bills_QTableView->setModel(database_model->billListModel());
            m_ui.m_bills_QTableView->setItemDelegate(new BillItemDelegate(this));
            m_ui.m_bills_QTableView->setEditTriggers(QTableView::EditTrigger::DoubleClicked);
            m_ui.m_bills_QTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
            m_database_model = database_model;
        }

    private slots:
        void slotNewBill() {
            Q_ASSERT(m_database_model != nullptr);

            auto *bill_model = m_database_model->billListModel()->createBill();

            BillEditorDialog dialog(bill_model);
            dialog.exec();
        }

    private:
        Ui::BillListWidget m_ui;

        Models::DatabaseModel *m_database_model = nullptr;
    };
}
