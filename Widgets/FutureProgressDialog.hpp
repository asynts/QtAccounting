#pragma once

#include <QDialog>
#include <QPushButton>

#include "ui_FutureProgressDialog.h"

namespace Accounting::Widgets
{
    class FutureProgressDialog final : public QDialog {
        Q_OBJECT

    public:
        FutureProgressDialog(
                QString windowTitle,
                std::function<void(std::function<void()> fulfill, std::function<void()> reject)>&& callback)
        {
            m_callback = std::move(callback);

            m_ui.setupUi(this);

            setWindowTitle(windowTitle);

            m_ui.m_description_QLabel->setText("Waiting for operation to complete.");

            m_ui.m_buttons_QDialogButtonBox->setStandardButtons(QDialogButtonBox::StandardButton::NoButton);

            connect(m_ui.m_buttons_QDialogButtonBox->button(QDialogButtonBox::StandardButton::Retry), &QPushButton::clicked,
                    this, &FutureProgressDialog::slotRetry);

            m_callback(std::bind(&FutureProgressDialog::slotFulfill, this), std::bind(&FutureProgressDialog::slotReject, this));
        }

    public slots:
        void slotFulfill()
        {
            // Automatically close the dialog.
            done(QDialog::DialogCode::Accepted);
        }

        void slotReject()
        {
            m_ui.m_description_QLabel->setText("Operation failed.");
            m_ui.m_buttons_QDialogButtonBox->setStandardButtons(QDialogButtonBox::StandardButton::Retry);
        }

    private slots:
        void slotRetry()
        {
            m_ui.m_description_QLabel->setText("Retrying operation; waiting for operation to complete.");
            m_ui.m_buttons_QDialogButtonBox->setStandardButtons(QDialogButtonBox::StandardButton::NoButton);
            m_callback(std::bind(&FutureProgressDialog::slotFulfill, this), std::bind(&FutureProgressDialog::slotReject, this));
        }

    private:
        std::function<void(std::function<void()> fulfill, std::function<void()> reject)> m_callback;
        Ui::FutureProgressDialog m_ui;
    };
}
