#include <QApplication>
#include <QBuffer>
#include <QVBoxLayout>
#include <QSettings>

#include <aws/core/Aws.h>
#include <aws/core/utils/logging/ConsoleLogSystem.h>

#include "Util.hpp"
#include "MainWindow.hpp"

void setup_AWS() {
}

int main(int argc, char *argv[])
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

    QApplication application(argc, argv);

    Accounting::MainWindow main_window;
    main_window.show();

    int retval = application.exec();

    Aws::ShutdownAPI(options);

    return retval;
}
