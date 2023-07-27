#include "MotorWindow.hpp"

#include <QApplication>
#include <QtPlugin>

#include <iostream>
#include <stdexcept>

#if __linux__
Q_IMPORT_PLUGIN(QXcbIntegrationPlugin)
#endif

// --------------------------------------------------------------------------
int main(int argc, char** argv)
{
    setenv("FONTCONFIG_PATH", "/etc/fonts", false);

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
