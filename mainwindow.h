#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <KXmlGuiWindow>
#include <QProcess>
#include <QTemporaryDir>
#include <QMap>

#include "portableappinfo.h"
#include "wineconfigwidget.h"
#include "flatpakmanifest.h"

class QListWidget;
class QStackedWidget;
class QProgressBar;
class QLabel;
class QPushButton;
class QLineEdit;

class MainWindow : public KXmlGuiWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void importPortableApp();
    void analyzePortableApp();
    void configureWineSettings();
    void generateFlatpakManifest();
    void buildFlatpak();
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void updateLog(const QString &message);
    void updateProgress(int value);
    void appSelected(int index);
    void removeSelectedApp();
    void browseForIcon();
    void updateIconPreview(const QString &path);

private:
    void setupActions();
    void setupUi();
    void setupConnections();
    bool checkDependencies();
    void loadSavedApps();
    void saveAppsList();
    bool prepareWinePrefix(const PortableAppInfo &appInfo);
    
    // UI Elements
    QStackedWidget *m_stackedWidget;
    QListWidget *m_appsList;
    QWidget *m_welcomePage;
    QWidget *m_appDetailsPage;
    QWidget *m_wineConfigPage;
    QWidget *m_buildPage;
    
    QLineEdit *m_appNameEdit;
    QLineEdit *m_appVersionEdit;
    QLineEdit *m_appDescriptionEdit;
    QLineEdit *m_appCategoryEdit;
    QLineEdit *m_executablePathEdit;
    QLineEdit *m_iconPathEdit;
    QPushButton *m_iconBrowseButton;
    QLabel *m_iconPreviewLabel;
    
    WineConfigWidget *m_wineConfigWidget;
    
    QProgressBar *m_progressBar;
    QLabel *m_statusLabel;
    QPushButton *m_importButton;
    QPushButton *m_analyzeButton;
    QPushButton *m_configureButton;
    QPushButton *m_buildButton;
    
    // Data
    QMap<QString, PortableAppInfo> m_portableApps;
    QString m_currentAppId;
    FlatpakManifest m_manifest;
    
    // Process and directories
    QProcess m_process;
    QTemporaryDir m_tempDir;
};

#endif // MAINWINDOW_H