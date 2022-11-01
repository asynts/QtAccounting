#pragma once

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QPushButton>

#include "Persistance/Database.hpp"

namespace Accounting::Widgets
{
    class BillListWidget final : public QWidget {
        Q_OBJECT

    public:
        BillListWidget(Persistance::Database& database, QWidget *parent = nullptr)
            : QWidget(parent)
            , m_database(database)
        {
            setLayout(new QVBoxLayout);

            onUpdate();

            connect(&m_database, &Persistance::Database::onBillAdded,
                    this, &BillListWidget::onUpdate);
        }

    private slots:
        void onUpdate() {
            auto *parent_widget = generate_parent_widget();

            if (m_parent_widget != nullptr) {
                layout()->removeWidget(m_parent_widget);
                delete m_parent_widget;
            }

            layout()->addWidget(parent_widget);
            m_parent_widget = parent_widget;
        }

    private:
        QWidget* generate_parent_widget() {
            auto parent_widget = new QWidget(this);

            auto parent_layout = new QVBoxLayout;
            parent_widget->setLayout(parent_layout);

            for (auto *bill_object : std::as_const(m_database.m_bills)) {
                auto *bill_widget = new QWidget(parent_widget);
                parent_layout->addWidget(bill_widget);

                auto *bill_layout = new QHBoxLayout;
                bill_widget->setLayout(bill_layout);

                if (bill_object == m_database.m_staged_bill) {
                    bill_layout->addWidget(new QLabel(QString("%1 (staged)").arg(bill_object->id()), parent_widget));
                } else {
                    bill_layout->addWidget(new QLabel(bill_object->id(), parent_widget));
                }

                auto *edit_button = new QPushButton("Edit", parent_widget);
                bill_layout->addWidget(edit_button);

                connect(edit_button, &QPushButton::clicked,
                        this, [=] {
                            // FIXME: Open dialog to edit this bill.
                        });
            }

            return parent_widget;
        }

        QWidget *m_parent_widget = nullptr;

        Persistance::Database& m_database;
    };
}
