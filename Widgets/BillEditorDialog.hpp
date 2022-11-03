#pragma once

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QPushButton>
#include <QDialog>

#include "Persistance/Database.hpp"
#include "Widgets/TransactionEditorDialog.hpp"
#include "ui_BillEditorDialog.h"

namespace Accounting::Widgets
{
    class TransactionItemWidget final : public QWidget {
        Q_OBJECT

    public:
        explicit TransactionItemWidget(Persistance::BillObject *bill_object, Persistance::TransactionObject *transaction_object, QWidget *parent = nullptr)
            : QWidget(parent)
            , m_bill_object(bill_object)
            , m_transaction_object(transaction_object)
        {
            auto *layout = new QHBoxLayout();
            setLayout(layout);

            layout->addWidget(new QLabel(transaction_object->id(), this));

            m_date_label = new QLabel(this);
            layout->addWidget(m_date_label);

            m_expense_label = new QLabel(this);
            layout->addWidget(m_expense_label);

            m_amount_label = new QLabel(this);
            layout->addWidget(m_amount_label);

            m_category_label = new QLabel(this);
            layout->addWidget(m_category_label);

            auto *edit_button = new QPushButton("Edit", this);
            layout->addWidget(edit_button);

            connect(edit_button, &QPushButton::clicked,
                    this, [=]() mutable {
                        TransactionEditorDialog dialog{ bill_object, transaction_object };
                        dialog.exec();
                    });

            connect(transaction_object, &Persistance::TransactionObject::signalChanged,
                    this, &TransactionItemWidget::slotUpdate);

            slotUpdate();
        }

    private slots:
        void slotUpdate() {
            m_date_label->setText(m_transaction_object->date().toString("yyyy-MM-dd"));

            QString expense;
            if (m_transaction_object->amount() <= 0.00) {
                expense = "EXPENSE";
            } else {
                expense = "INCOME";
            }
            m_expense_label->setText(expense);

            auto amount = std::abs(m_transaction_object->amount());
            m_amount_label->setText(QString::number(amount, 'f', 2));

            m_category_label->setText(m_transaction_object->category());
        }

    private:
        QLabel *m_date_label;
        QLabel *m_amount_label;
        QLabel *m_category_label;
        QLabel *m_expense_label;

        Persistance::BillObject *m_bill_object;
        Persistance::TransactionObject *m_transaction_object;
    };

    class BillEditorDialog final : public QDialog {
        Q_OBJECT

    public:
        explicit BillEditorDialog(Persistance::BillObject *bill_object, QWidget *parent = nullptr)
            : QDialog(parent)
            , m_bill_object(bill_object)
        {
            m_ui.setupUi(this);

            setWindowTitle(QString("Edit Bill '%1'").arg(bill_object->id()));

            fill_QComboBox_with_enum<Persistance::BillObject::Status>(m_ui.m_status_QComboBox);

            connect(m_ui.m_status_QComboBox, &QComboBox::currentIndexChanged,
                    this, &BillEditorDialog::slotStatusChanged);

            connect(m_ui.m_new_QPushButton, &QPushButton::clicked,
                    this, &BillEditorDialog::slotNewTransaction);

            connect(bill_object, &Persistance::BillObject::signalChanged,
                    this, &BillEditorDialog::slotUpdate);

            slotUpdate();
        }

    private slots:
        void slotUpdate() {
            // This is not an infinite loop, since we only emit 'signalStatusChanged' if the index changes.
            m_ui.m_status_QComboBox->setCurrentIndex(static_cast<int>(m_bill_object->status()));

            auto *container_widget = generateContainerWidget();
            layout()->replaceWidget(m_ui.m_container_QWidget, container_widget);

            m_ui.m_container_QWidget->deleteLater();
            m_ui.m_container_QWidget = container_widget;

            dumpObjectTree();
        }

        void slotStatusChanged() {
            m_bill_object->setStatus(static_cast<Persistance::BillObject::Status>(m_ui.m_status_QComboBox->currentIndex()));
        }

        void slotNewTransaction()
        {
            TransactionEditorDialog dialog{ m_bill_object, nullptr };
            dialog.exec();
        }

    private:
        QWidget* generateContainerWidget() {
            auto container_widget = new QWidget(this);
            container_widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

            auto container_layout = new QVBoxLayout;
            container_layout->setAlignment(Qt::AlignTop);
            container_widget->setLayout(container_layout);

            for (auto *transaction_object : m_bill_object->transactions()) {
                auto *transaction_widget = new TransactionItemWidget(m_bill_object, transaction_object, container_widget);
                container_layout->addWidget(transaction_widget);
            }

            return container_widget;
        }

        Ui::BillEditorDialog m_ui;

        Persistance::BillObject *const m_bill_object;
    };
}
