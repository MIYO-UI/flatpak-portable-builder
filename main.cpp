#include <QApplication>
#include <QCommandLineParser>
#include <KAboutData>
#include <KLocalizedString>
#include <KCrash>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    KCrash::initialize();
    
    KLocalizedString::setApplicationDomain("flatpack-portable-builder");
    
    KAboutData aboutData(
        // Program name
        QStringLiteral("flatpack-portable-builder"),
        // Translatable program name
        i18n("Flatpak Portable Builder"),
        // Program version
        QStringLiteral("1.0"),
        // Short description
        i18n("Build Flatpaks from Windows PortableApps"),
        // License
        KAboutLicense::GPL_V3,
        // Copyright statement
        i18n("(c) 2025"),
        // Additional text
        i18n("A tool to convert Windows PortableApps to Flatpaks using Wine"),
        // Homepage
        QStringLiteral("https://github.com/yourusername/flatpack-portable-builder"),
        // Bug reporting address
        QStringLiteral("your@email.com")
    );
    
    aboutData.addAuthor(
        i18n("Your Name"),
        i18n("Developer"),
        QStringLiteral("your@email.com"),
        QStringLiteral("https://yourwebsite.com")
    );
    
    KAboutData::setApplicationData(aboutData);
    
    QCommandLineParser parser;
    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);
    
    MainWindow *window = new MainWindow();
    window->show();
    
    return app.exec();
}