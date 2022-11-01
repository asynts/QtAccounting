#pragma once

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QPushButton>

#include "Persistance/Database.hpp"
#include "Widgets/BillEditorDialog.hpp"
#include "ui_BillListWidget.h"

namespace Accounting::Widgets
{
    class BillListWidget final : public QWidget {
        Q_OBJECT

    public:
        BillListWidget(Persistance::Database& database, QWidget *parent = nullptr)
            : QWidget(parent)
            , m_database(database)
        {
            m_ui.setupUi(this);

            connect(&m_database, &Persistance::Database::signalBillChanged,
                    this, &BillListWidget::slotUpdate);

            connect(m_ui.m_new_QPushButton, &QPushButton::clicked,
                    this, &BillListWidget::slotNewBill);

            slotUpdate();
        }

    private slots:
        void slotUpdate() {
            auto *container_widget = generateContainerWidget();
            layout()->replaceWidget(m_ui.m_container_QWidget, container_widget);

            delete m_ui.m_container_QWidget;
            m_ui.m_container_QWidget = container_widget;
        }

        void slotNewBill() {
            m_database.create_staged_bill();
        }

    private:
        QWidget* generateContainerWidget() {
            auto container_widget = new QWidget(this);

            auto container_layout = new QVBoxLayout;
            container_layout->setAlignment(Qt::AlignTop);
            container_widget->setLayout(container_layout);

            for (auto *bill_object : std::as_const(m_database.m_bills)) {
                auto *bill_widget = new QWidget(container_widget);
                container_layout->addWidget(bill_widget);

                auto *bill_layout = new QHBoxLayout;
                bill_widget->setLayout(bill_layout);

                bill_layout->addWidget(new QLabel(QString("%1 (%2)").arg(bill_object->id(), bill_object->status_string()), container_widget));

                auto *edit_button = new QPushButton("Edit", container_widget);
                bill_layout->addWidget(edit_button);

                BillEditorDialog *dialog = nullptr;
                connect(edit_button, &QPushButton::clicked,
                        this, [=]() mutable {
                            if (dialog == nullptr) {
                                dialog = new BillEditorDialog(*bill_object, container_widget);
                            }

                            dialog->show();
                            dialog->raise();
                            dialog->activateWindow();
                        });
            }

            return container_widget;
        }

        Ui::BillListWidget m_ui;

        Persistance::Database& m_database;
    };
}
