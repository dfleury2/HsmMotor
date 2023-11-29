#include "TricolorMainWindow.hpp"

#include <QApplication>
#include <QtPlugin>

#include <iostream>

#ifdef __linux__
Q_IMPORT_PLUGIN(QXcbIntegrationPlugin)
#endif

// --------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    try {
        // Can be put in .bashrc or .profile
        setenv("FONTCONFIG_PATH", "/etc/fonts", false);

        QApplication app(argc, argv);

        TricolorMainWindow window;
        window.show();

        return QCoreApplication::exec();
    }
    catch (const std::exception& ex) {
        std::cout << ex.what() << std::endl;
    }
}
