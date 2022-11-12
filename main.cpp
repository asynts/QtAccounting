#include <QApplication>
#include <QBuffer>
#include <QVBoxLayout>
#include <QSettings>

#include <aws/core/Aws.h>

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
    options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Trace;
    Aws::InitAPI(options);

    QApplication application(argc, argv);

    Accounting::MainWindow main_window;
    main_window.show();

    int retval = application.exec();

    Aws::ShutdownAPI(options);

    return retval;
}
