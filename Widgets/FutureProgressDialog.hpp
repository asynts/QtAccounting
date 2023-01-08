#pragma once

#include <future>

#include <QDialog>
#include <QPushButton>

#include "ui_FutureProgressDialog.h"

namespace Accounting::Widgets
{
    class FutureProgressDialog final : public QDialog {
        Q_OBJECT

    public:
        enum class Result {
            Success,
            Failure,
        };

        explicit FutureProgressDialog(
                QString windowTitle,
                std::function<std::future<Result>()>&& callback)
        {
            m_callback = std::move(callback);

            m_ui.setupUi(this);

            setWindowTitle(windowTitle);

            connect(m_ui.m_buttons_QDialogButtonBox->button(QDialogButtonBox::StandardButton::Retry), &QPushButton::clicked,
                    this, &FutureProgressDialog::slotRetry);

            m_ui.m_description_QLabel->setText("Waiting for operation to complete.");
            m_ui.m_buttons_QDialogButtonBox->setStandardButtons(QDialogButtonBox::StandardButton::NoButton);

            try_operation();
        }

    private slots:
        void slotRetry()
        {
            m_ui.m_description_QLabel->setText("Retrying operation; waiting for operation to complete.");
            m_ui.m_buttons_QDialogButtonBox->setStandardButtons(QDialogButtonBox::StandardButton::NoButton);

            try_operation();
        }

    private:
        void try_operation() {
            m_callback_future = std::async(std::launch::async, [this] {
                auto result = m_callback().get();
                if (result == Result::Success) {
                    // Automatically close the dialog.
                    done(QDialog::DialogCode::Accepted);
                } else {
                    m_ui.m_description_QLabel->setText("Operation failed.");
                    m_ui.m_buttons_QDialogButtonBox->setStandardButtons(QDialogButtonBox::StandardButton::Retry);
                }
            });
        }

        std::function<std::future<Result>()> m_callback;
        std::future<void> m_callback_future;
        Ui::FutureProgressDialog m_ui;
    };
}
