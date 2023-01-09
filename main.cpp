#include <QApplication>
#include <QBuffer>
#include <QVBoxLayout>
#include <QSettings>

#include <aws/core/Aws.h>
#include <aws/core/utils/logging/ConsoleLogSystem.h>

#include "Util.hpp"
#include "MainWindow.hpp"

namespace Accounting
{
    int gui_main(int argc, char **argv)
    {
        QApplication application(argc, argv);

        auto main_window_opt = Accounting::MainWindow::try_create_main_window();

        if (!main_window_opt.has_value()) {
            // The 'try_create_main_window' function already provided feedback to the user.
            return 0;
        }

        main_window_opt.value()->show();

        return application.exec();
    }
}

int main(int argc, char **argv)
{
    // Needed to use 'QSettings'.
    QCoreApplication::setOrganizationName("Asynts");
    QCoreApplication::setOrganizationDomain("asynts.com");

#ifdef QT_DEBUG
    QCoreApplication::setApplicationName("Accounting-Debug");
#else
    QCoreApplication::setApplicationName("Accounting");
#endif

    // Setup AWS.
    Aws::SDKOptions options;
    options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Warn;
    options.loggingOptions.logger_create_fn = []() {
        return Aws::MakeShared<Aws::Utils::Logging::ConsoleLogSystem>(Accounting::ACCOUNTING_ALLOCATION_TAG, Aws::Utils::Logging::LogLevel::Warn);
    };
    Aws::InitAPI(options);

    int retval = Accounting::gui_main(argc, argv);

    // Cleanup AWS.
    Aws::ShutdownAPI(options);

    return retval;
}
