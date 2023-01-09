#pragma once

#include <future>

#include <QDialog>
#include <QPushButton>

#include "Events.hpp"

#include "ui_FutureProgressDialog.h"

namespace Accounting::Widgets
{
    class FutureProgressDialog final : public QDialog {
        Q_OBJECT

    public:
        enum class StateEnum {
            // The future has not yet been resolved.
            // The user can cancel, but we wait for the operation to complete.
            Waiting,

            // The future was resolved as a failure, but the user retried the operation.
            // The user can cancel, but we wait for the operation to complete.
            WaitingAgain,

            // The future was resolved as a failure.
            // The user can retry or cancel.
            Failed,
        };

        explicit FutureProgressDialog(
                QString windowTitle,
                std::function<std::future<ResultEnum>()>&& callback)
        {
            m_callback = std::move(callback);

            m_ui.setupUi(this);

            setWindowTitle(windowTitle);

            m_state = StateEnum::Waiting;
            updateInterface();

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
        void slotCancel()
        {
            // The destructor of the 'm_callback_future' will wait for it to complete.
            close();
        }

        void slotRetry()
        {
            Q_ASSERT(m_state == StateEnum::Failed);
            m_state = StateEnum::WaitingAgain;
            updateInterface();

            try_operation();
        }

    private:
        void updateInterface()
        {
            using StandardButton = QDialogButtonBox::StandardButton;

            if (m_state == StateEnum::Waiting) {
                m_ui.m_description_QLabel->setText("Waiting for operation to complete.");
                m_ui.m_buttons_QDialogButtonBox->setStandardButtons(StandardButton::Cancel);
            } else if (m_state == StateEnum::Failed) {
                m_ui.m_description_QLabel->setText("Operation failed.");
                m_ui.m_buttons_QDialogButtonBox->setStandardButtons(StandardButton::Cancel | StandardButton::Retry);

                connect(m_ui.m_buttons_QDialogButtonBox->button(QDialogButtonBox::StandardButton::Retry), &QPushButton::clicked,
                        this, &FutureProgressDialog::slotRetry);
            } else if (m_state == StateEnum::WaitingAgain) {
                m_ui.m_description_QLabel->setText("Retrying operation; waiting for operation to complete.");
                m_ui.m_buttons_QDialogButtonBox->setStandardButtons(StandardButton::Cancel);
            } else {
                Q_UNREACHABLE();
            }

            connect(m_ui.m_buttons_QDialogButtonBox->button(QDialogButtonBox::StandardButton::Cancel), &QPushButton::clicked,
                    this, &FutureProgressDialog::slotCancel);
        }

        void eventFutureReady(ResultEnum result)
        {
            if (result == ResultEnum::Success) {
                // Automatically close the dialog.
                done(QDialog::DialogCode::Accepted);
            } else {
                m_state = StateEnum::Failed;
                updateInterface();
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

        std::function<std::future<ResultEnum>()> m_callback;
        std::future<void> m_callback_future;

        Ui::FutureProgressDialog m_ui;

        StateEnum m_state;
    };
}
