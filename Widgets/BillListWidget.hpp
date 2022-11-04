#pragma once

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QPushButton>
#include <QStyledItemDelegate>
#include <QPainter>

#include "Models/DatabaseModel.hpp"
/*
#include "Widgets/BillEditorDialog.hpp"
*/

#include "ui_BillListWidget.h"

namespace Accounting::Widgets
{
    class BillItemDelegate final : public QStyledItemDelegate {
        Q_OBJECT

    public:
        explicit BillItemDelegate(QObject *parent = nullptr)
            : QStyledItemDelegate(parent) { }

        // FIXME: How can I associate the button with the 'QModelIndex'?
        //        Can I attach data to the 'QModelIndex' directly?

        virtual void paint(QPainter *painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override {
            // Not 'Edit' column.
            if (index.column() != 2) {
                return QStyledItemDelegate::paint(painter, option, index);
            }

            // FIXME
            painter->fillRect(option.rect, Qt::red);
        }

        virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override {
            // Not 'Edit' column.
            if (index.column() != 2) {
                return QStyledItemDelegate::sizeHint(option, index);
            }

            // FIXME
            return QSize(0, 0);
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
