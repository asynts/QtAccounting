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
    class BillItemWidget final : public QWidget {
        Q_OBJECT

    public:
        explicit BillItemWidget(Persistance::BillObject *bill_object, QWidget *parent)
            : QWidget(parent)
            , m_bill_object(bill_object)
        {
            auto *layout = new QHBoxLayout();
            setLayout(layout);

            m_label_widget = new QLabel(this);
            layout->addWidget(m_label_widget);

            auto *edit_button = new QPushButton("Edit", this);
            layout->addWidget(edit_button);

            connect(edit_button, &QPushButton::clicked,
                this, [=]() {
                    BillEditorDialog dialog{ bill_object };
                    dialog.exec();
                });

            connect(m_bill_object, &Persistance::BillObject::signalChanged,
                    this, &BillItemWidget::slotUpdate);

            slotUpdate();
        }

    private slots:
        void slotUpdate() {
            auto label_text = QString("%1 (%2)").arg(
                m_bill_object->id(),
                Persistance::BillObject::status_to_string(m_bill_object->status())
            );

            m_label_widget->setText(label_text);
        }

    private:
        QLabel *m_label_widget;

        Persistance::BillObject *m_bill_object;
    };

    class BillListWidget final : public QWidget {
        Q_OBJECT

    public:
        explicit BillListWidget(Persistance::DatabaseObject *database_object, QWidget *parent = nullptr)
            : QWidget(parent)
            , m_database_object(database_object)
        {
            m_ui.setupUi(this);

            connect(m_database_object, &Persistance::DatabaseObject::signalChanged,
                    this, &BillListWidget::slotUpdate);

            connect(m_ui.m_new_QPushButton, &QPushButton::clicked,
                    this, &BillListWidget::slotNewBill);

            slotUpdate();
        }

    private slots:
        void slotUpdate() {
            auto *container_widget = generateContainerWidget();
            layout()->replaceWidget(m_ui.m_container_QWidget, container_widget);

            m_ui.m_container_QWidget->deleteLater();
            m_ui.m_container_QWidget = container_widget;
        }

        void slotNewBill() {
            m_database_object->createBill();
        }

    private:
        QWidget* generateContainerWidget() {
            auto container_widget = new QWidget(this);
            container_widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

            auto container_layout = new QVBoxLayout;
            container_layout->setAlignment(Qt::AlignTop);
            container_widget->setLayout(container_layout);

            for (auto *bill_object : m_database_object->bills()) {
                auto *bill_widget = new BillItemWidget(bill_object, container_widget);
                container_layout->addWidget(bill_widget);
            }

            return container_widget;
        }

        Ui::BillListWidget m_ui;

        Persistance::DatabaseObject *m_database_object;
    };
}
