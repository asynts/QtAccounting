#pragma once

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QPushButton>
#include <QStyledItemDelegate>
#include <QPainter>

#include "Models/DatabaseModel.hpp"
#include "Widgets/BillEditorDialog.hpp"

#include "ui_BillListWidget.h"

namespace Accounting::Widgets
{
    class BillItemDelegate final : public QStyledItemDelegate {
        Q_OBJECT

    public:
        explicit BillItemDelegate(QObject *parent = nullptr)
            : QStyledItemDelegate(parent) { }

        virtual bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem& option, const QModelIndex& index) override {
            qDebug() << event->type();

            if (!model->checkIndex(index)) {
                return false;
            }

            // All mouse events are delivered here for some reason.
            // The name 'editorEvent' is misleading.
            if (event->type() != QEvent::MouseButtonDblClick) {
                return false;
            }

            auto *bill_model = reinterpret_cast<Models::BillModel*>(index.internalPointer());

            BillEditorDialog dialog(bill_model);
            dialog.exec();

            return true;
        }
    };

    class BillListWidget final : public QWidget {
        Q_OBJECT

    public:
        explicit BillListWidget(Models::DatabaseModel *database_model, QWidget *parent = nullptr)
            : QWidget(parent)
            , m_database_model(database_model)
        {
            m_ui.setupUi(this);

            m_ui.m_bills_QTableView->setModel(database_model);
            m_ui.m_bills_QTableView->setItemDelegate(new BillItemDelegate(this));
            m_ui.m_bills_QTableView->setEditTriggers(QTableView::EditTrigger::DoubleClicked);

            connect(m_ui.m_new_QPushButton, &QPushButton::clicked,
                    this, &BillListWidget::slotNewBill);
        }

    private slots:
        void slotNewBill() {
            m_database_model->createBill();
        }

    private:
        Ui::BillListWidget m_ui;

        Models::DatabaseModel *m_database_model;
    };
}
