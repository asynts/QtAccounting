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

        virtual QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override {
            // FIXME: Obtain 'BillModel' from 'DatabaseModel'
            //        Maybe I can use another 'role' for that?
            Models::BillModel *bill_model = nullptr;

            return new BillEditorDialog(bill_model, parent);
        }

        // FIXME: What is this even supposed to do?
        virtual void setEditorData(QWidget *editor, const QModelIndex &index) const override {
            QStyledItemDelegate::setEditorData(editor, index);
        }

        // FIXME: What is this even supposed to do?
        virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override {
            QStyledItemDelegate::setModelData(editor, model, index);
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
