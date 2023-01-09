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

        class FutureReadyEvent : public QEvent {
        public:
            explicit FutureReadyEvent(Result result)
                : QEvent(FutureReadyEvent::Type)
                , m_result(result) { }

            static constexpr QEvent::Type Type = static_cast<QEvent::Type>(QEvent::User + 1);

            Result m_result;
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

        virtual bool event(QEvent *event) override
        {
            if (event->type() == FutureReadyEvent::Type) {
                eventFutureReady(static_cast<FutureReadyEvent*>(event)->m_result);
                return true;
            } else {
                return QDialog::event(event);
            }
        }

    private slots:
        void slotRetry()
        {
            m_ui.m_description_QLabel->setText("Retrying operation; waiting for operation to complete.");
            m_ui.m_buttons_QDialogButtonBox->setStandardButtons(QDialogButtonBox::StandardButton::NoButton);

            try_operation();
        }

    private:
        void eventFutureReady(Result result)
        {
            if (result == Result::Success) {
                // Automatically close the dialog.
                done(QDialog::DialogCode::Accepted);
            } else {
                // Allow the user to retry.
                m_ui.m_description_QLabel->setText("Operation failed.");
                m_ui.m_buttons_QDialogButtonBox->setStandardButtons(QDialogButtonBox::StandardButton::Retry);
            }
        }

        void try_operation() {
            m_callback_future = std::async(std::launch::async, [this] {
                // We must not update the UI from a worker thread.
                // Emit an event for the main thread.
                auto result = m_callback().get();
                QCoreApplication::postEvent(this, new FutureReadyEvent{ result });
            });
        }

        std::function<std::future<Result>()> m_callback;
        std::future<void> m_callback_future;
        Ui::FutureProgressDialog m_ui;
    };
}
