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
    QCoreApplication::setApplicationName("Accounting");

    // FIXME: Where does AWS get the authentication token from?

    // Setup AWS.
    Aws::SDKOptions options;
    options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Debug;
    InitAPI(options);

    QApplication application(argc, argv);

    Accounting::MainWindow main_window;
    main_window.show();

    return application.exec();
}
