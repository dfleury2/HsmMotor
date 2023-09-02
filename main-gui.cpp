#include "MotorWindow.hpp"

#include <QApplication>
#include <QtPlugin>

#include <iostream>
#include <stdexcept>

#if __linux__
Q_IMPORT_PLUGIN(QXcbIntegrationPlugin)
#else
Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
#endif


// --------------------------------------------------------------------------
int main(int argc, char** argv)
{
#if __linux__
    setenv("FONTCONFIG_PATH", "/etc/fonts", false);
#endif
    try {
        QApplication app(argc, argv);

        // Show the main window
        MotorWindow mainWindow;
        mainWindow.show();

        return QCoreApplication::exec();
    }
    catch (const std::exception& ex) {
        std::cout << ex.what() << std::endl;
    }
}
