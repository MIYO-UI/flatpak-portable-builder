#include "mainwindow.h"

#include <KActionCollection>
#include <KLocalizedString>
#include <KMessageBox>
#include <KStandardAction>

#include <QListWidget>
#include <QStackedWidget>
#include <QProgressBar>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QSettings>
#include <QDir>
#include <QUuid>
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

MainWindow::MainWindow(QWidget *parent)
    : KXmlGuiWindow(parent)
{
    // Setup UI first
    setupUi();
    
    // Setup actions
    setupActions();
    
    // Setup connections
    setupConnections();
    
    // Check for required tools
    if (!checkDependencies()) {
        KMessageBox::error(this, 
            i18n("Missing dependencies. Please install flatpak-builder, wine, and required tools."), 
            i18n("Dependency Error"));
    }
    
    // Load any saved applications
    loadSavedApps();
    
    // Set window properties
    setWindowTitle(i18n("Flatpak Portable Builder"));
    setMinimumSize(800, 600);
    
    // Show welcome page initially
    m_stackedWidget->setCurrentIndex(0);
}

MainWindow::~MainWindow()
{
    // Save the applications list before exiting
    saveAppsList();
}

void MainWindow::setupUi()
{
    // Main layout with central widget
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    
    // Left side with apps list
    QVBoxLayout *leftLayout = new QVBoxLayout();
    QLabel *appsLabel = new QLabel(i18n("Portable Apps"));
    m_appsList = new QListWidget();
    
    QPushButton *addAppButton = new QPushButton(i18n("Import New App"));
    QPushButton *removeAppButton = new QPushButton(i18n("Remove App"));
    
    QHBoxLayout *appButtonsLayout = new QHBoxLayout();
    appButtonsLayout->addWidget(addAppButton);
    appButtonsLayout->addWidget(removeAppButton);
    
    leftLayout->addWidget(appsLabel);
    leftLayout->addWidget(m_appsList);
    leftLayout->addLayout(appButtonsLayout);
    
    // Right side with stacked pages
    m_stackedWidget = new QStackedWidget();
    
    // Welcome page
    m_welcomePage = new QWidget();
    QVBoxLayout *welcomeLayout = new QVBoxLayout(m_welcomePage);
    QLabel *welcomeLabel = new QLabel(i18n("Welcome to Flatpak Portable Builder"));
    QLabel *welcomeDescLabel = new QLabel(i18n("This tool helps you convert Windows PortableApps to Flatpaks that run using Wine."));
    welcomeLayout->addWidget(welcomeLabel);
    welcomeLayout->addWidget(welcomeDescLabel);
    welcomeLayout->addStretch();
    m_stackedWidget->addWidget(m_welcomePage);
    
    // App details page
    m_appDetailsPage = new QWidget();
    QVBoxLayout *detailsLayout = new QVBoxLayout(m_appDetailsPage);
    
    QGroupBox *appInfoGroup = new QGroupBox(i18n("Application Information"));
    QFormLayout *formLayout = new QFormLayout(appInfoGroup);
    
    m_appNameEdit = new QLineEdit();
    m_appVersionEdit = new QLineEdit();
    m_appDescriptionEdit = new QLineEdit();
    m_appCategoryEdit = new QLineEdit();
    m_executablePathEdit = new QLineEdit();
    
    formLayout->addRow(i18n("Name:"), m_appNameEdit);
    formLayout->addRow(i18n("Version:"), m_appVersionEdit);
    formLayout->addRow(i18n("Description:"), m_appDescriptionEdit);
    formLayout->addRow(i18n("Category:"), m_appCategoryEdit);
    formLayout->addRow(i18n("Executable Path:"), m_executablePathEdit);
    
    // Icon selection
    QHBoxLayout *iconLayout = new QHBoxLayout();
    m_iconPathEdit = new QLineEdit();
    m_iconBrowseButton = new QPushButton(i18n("Browse..."));
    iconLayout->addWidget(m_iconPathEdit);
    iconLayout->addWidget(m_iconBrowseButton);
    
    // Icon preview
    m_iconPreviewLabel = new QLabel();
    m_iconPreviewLabel->setFixedSize(48, 48);
    m_iconPreviewLabel->setAlignment(Qt::AlignCenter);
    m_iconPreviewLabel->setFrameShape(QFrame::StyledPanel);
    
    QHBoxLayout *previewLayout = new QHBoxLayout();
    previewLayout->addLayout(iconLayout);
    previewLayout->addWidget(m_iconPreviewLabel);
    
    formLayout->addRow(i18n("Icon:"), previewLayout);
    
    m_analyzeButton = new QPushButton(i18n("Analyze App"));
    
    detailsLayout->addWidget(appInfoGroup);
    detailsLayout->addWidget(m_analyzeButton);
    detailsLayout->addStretch();
    m_stackedWidget->addWidget(m_appDetailsPage);
    
    // Wine config page
    m_wineConfigPage = new QWidget();
    QVBoxLayout *wineConfigLayout = new QVBoxLayout(m_wineConfigPage);
    
    m_wineConfigWidget = new WineConfigWidget();
    m_configureButton = new QPushButton(i18n("Generate Flatpak Manifest"));
    
    wineConfigLayout->addWidget(m_wineConfigWidget);
    wineConfigLayout->addWidget(m_configureButton);
    wineConfigLayout->addStretch();
    m_stackedWidget->addWidget(m_wineConfigPage);
    
    // Build page
    m_buildPage = new QWidget();
    QVBoxLayout *buildLayout = new QVBoxLayout(m_buildPage);
    
    QLabel *buildLabel = new QLabel(i18n("Build Flatpak"));
    m_statusLabel = new QLabel(i18n("Ready to build..."));
    m_progressBar = new QProgressBar();
    m_buildButton = new QPushButton(i18n("Build Flatpak"));
    
    buildLayout->addWidget(buildLabel);
    buildLayout->addWidget(m_statusLabel);
    buildLayout->addWidget(m_progressBar);
    buildLayout->addWidget(m_buildButton);
    buildLayout->addStretch();
    m_stackedWidget->addWidget(m_buildPage);
    
    // Add the layouts to the main layout
    mainLayout->addLayout(leftLayout, 1);
    mainLayout->addWidget(m_stackedWidget, 3);
    
    // Connect UI elements
    connect(addAppButton, &QPushButton::clicked, this, &MainWindow::importPortableApp);
    connect(removeAppButton, &QPushButton::clicked, this, &MainWindow::removeSelectedApp);
    connect(m_analyzeButton, &QPushButton::clicked, this, &MainWindow::analyzePortableApp);
    connect(m_configureButton, &QPushButton::clicked, this, &MainWindow::generateFlatpakManifest);
    connect(m_buildButton, &QPushButton::clicked, this, &MainWindow::buildFlatpak);
    connect(m_appsList, &QListWidget::currentRowChanged, this, &MainWindow::appSelected);
    
    // Icon connections
    connect(m_iconBrowseButton, &QPushButton::clicked, this, &MainWindow::browseForIcon);
    connect(m_iconPathEdit, &QLineEdit::textChanged, this, &MainWindow::updateIconPreview);
}

void MainWindow::setupActions()
{
    KActionCollection *actionCollection = this->actionCollection();
    
    // Standard actions
    KStandardAction::quit(qApp, &QApplication::quit, actionCollection);
    KStandardAction::preferences(this, [this]() {
        // Placeholder for preferences dialog
        KMessageBox::information(this, i18n("Preferences dialog would appear here."), i18n("Preferences"));
    }, actionCollection);
    
    // Custom actions
    QAction *importAction = actionCollection->addAction(QStringLiteral("import_app"));
    importAction->setText(i18n("Import PortableApp"));
    importAction->setIcon(QIcon::fromTheme(QStringLiteral("document-import")));
    connect(importAction, &QAction::triggered, this, &MainWindow::importPortableApp);
    
    QAction *buildAction = actionCollection->addAction(QStringLiteral("build_flatpak"));
    buildAction->setText(i18n("Build Flatpak"));
    buildAction->setIcon(QIcon::fromTheme(QStringLiteral("run-build")));
    connect(buildAction, &QAction::triggered, this, &MainWindow::buildFlatpak);
    
    // Setup XML GUI
    setupGUI(Default, "flatpack-portable-builderui.rc");
}

void MainWindow::setupConnections()
{
    // Connect process signals
    connect(&m_process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
            this, &MainWindow::processFinished);
    
    connect(&m_process, &QProcess::readyReadStandardOutput, [this]() {
        QString output = QString::fromLocal8Bit(m_process.readAllStandardOutput());
        updateLog(output);
    });
    
    connect(&m_process, &QProcess::readyReadStandardError, [this]() {
        QString error = QString::fromLocal8Bit(m_process.readAllStandardError());
        updateLog(error);
    });
}

bool MainWindow::checkDependencies()
{
    // Check for flatpak-builder
    QProcess process;
    process.start("flatpak-builder", QStringList() << "--version");
    if (!process.waitForFinished() || process.exitCode() != 0) {
        updateLog(i18n("flatpak-builder not found!"));
        return false;
    }
    
    // Check for wine
    process.start("wine", QStringList() << "--version");
    if (!process.waitForFinished() || process.exitCode() != 0) {
        updateLog(i18n("wine not found!"));
        return false;
    }
    
    // Check for other dependencies...
    // bsdtar (for extracting archives)
    process.start("bsdtar", QStringList() << "--version");
    if (!process.waitForFinished() || process.exitCode() != 0) {
        updateLog(i18n("bsdtar not found!"));
        return false;
    }
    
    return true;
}

void MainWindow::loadSavedApps()
{
    QSettings settings;
    int size = settings.beginReadArray("portableApps");
    
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        PortableAppInfo info;
        info.id = settings.value("id").toString();
        info.name = settings.value("name").toString();
        info.version = settings.value("version").toString();
        info.description = settings.value("description").toString();
        info.category = settings.value("category").toString();
        info.sourceDir = settings.value("sourceDir").toString();
        info.executablePath = settings.value("executablePath").toString();
        info.wineVersion = settings.value("wineVersion").toString();
        info.wineDllOverrides = settings.value("wineDllOverrides").toString();
        
        m_portableApps[info.id] = info;
        m_appsList->addItem(info.name + " (" + info.version + ")");
    }
    
    settings.endArray();
}

void MainWindow::saveAppsList()
{
    QSettings settings;
    settings.beginWriteArray("portableApps");
    
    int i = 0;
    for (auto it = m_portableApps.constBegin(); it != m_portableApps.constEnd(); ++it) {
        settings.setArrayIndex(i++);
        const PortableAppInfo &info = it.value();
        
        settings.setValue("id", info.id);
        settings.setValue("name", info.name);
        settings.setValue("version", info.version);
        settings.setValue("description", info.description);
        settings.setValue("category", info.category);
        settings.setValue("sourceDir", info.sourceDir);
        settings.setValue("executablePath", info.executablePath);
        settings.setValue("wineVersion", info.wineVersion);
        settings.setValue("wineDllOverrides", info.wineDllOverrides);
    }
    
    settings.endArray();
}

void MainWindow::importPortableApp()
{
    QString dirPath = QFileDialog::getExistingDirectory(this, i18n("Select Portable App Directory"));
    if (dirPath.isEmpty())
        return;
    
    // Generate a unique ID for this app
    QString appId = QUuid::createUuid().toString(QUuid::WithoutBraces);
    
    // Create initial app info
    PortableAppInfo appInfo;
    appInfo.id = appId;
    appInfo.sourceDir = dirPath;
    
    // Try to detect information from directory structure
    QDir dir(dirPath);
    appInfo.name = dir.dirName();
    
    // Look for .exe files
    QStringList exeFiles;
    QDirIterator it(dirPath, QStringList() << "*.exe", QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        it.next();
        exeFiles << it.filePath();
    }
    
    // If we find any exe files, select the first as default
    if (!exeFiles.isEmpty()) {
        appInfo.executablePath = exeFiles.first();
    }
    
    // Look for icons
    QStringList iconFiles;
    QDirIterator iconIt(dirPath, QStringList() << "*.png" << "*.ico" << "*.svg" << "*.jpg", 
                      QDir::Files, QDirIterator::Subdirectories);
    while (iconIt.hasNext()) {
        iconIt.next();
        iconFiles << iconIt.filePath();
    }
    
    // Try to find an icon that contains common names
    QStringList iconKeywords = {"icon", "logo", appInfo.name.toLower()};
    
    // First try to find best matching icon
    for (const QString &keyword : iconKeywords) {
        for (const QString &iconFile : iconFiles) {
            if (QFileInfo(iconFile).fileName().toLower().contains(keyword)) {
                appInfo.iconPath = iconFile;
                break;
            }
        }
        if (!appInfo.iconPath.isEmpty())
            break;
    }
    
    // If no icon found yet, just use the first one
    if (appInfo.iconPath.isEmpty() && !iconFiles.isEmpty()) {
        appInfo.iconPath = iconFiles.first();
    }
    
    // Store the app info
    m_portableApps[appId] = appInfo;
    
    // Add to the list and select it
    int newRow = m_appsList->count();
    m_appsList->addItem(appInfo.name);
    m_appsList->setCurrentRow(newRow);
    
    // Switch to app details page
    m_currentAppId = appId;
    m_stackedWidget->setCurrentIndex(1);
    
    // Update the UI fields
    m_appNameEdit->setText(appInfo.name);
    m_appVersionEdit->setText(appInfo.version);
    m_appDescriptionEdit->setText(appInfo.description);
    m_appCategoryEdit->setText(appInfo.category);
    m_executablePathEdit->setText(appInfo.executablePath);
    m_iconPathEdit->setText(appInfo.iconPath);
    
    // Update icon preview
    updateIconPreview(appInfo.iconPath);
}

void MainWindow::analyzePortableApp()
{
    if (m_currentAppId.isEmpty() || !m_portableApps.contains(m_currentAppId)) {
        KMessageBox::error(this, i18n("No application selected!"), i18n("Error"));
        return;
    }
    
    // Update app info from UI fields
    PortableAppInfo &appInfo = m_portableApps[m_currentAppId];
    appInfo.name = m_appNameEdit->text();
    appInfo.version = m_appVersionEdit->text();
    appInfo.description = m_appDescriptionEdit->text();
    appInfo.category = m_appCategoryEdit->text();
    appInfo.executablePath = m_executablePathEdit->text();
    appInfo.iconPath = m_iconPathEdit->text();
    
    // Update list item
    int currentRow = m_appsList->currentRow();
    m_appsList->item(currentRow)->setText(appInfo.name + " (" + appInfo.version + ")");
    
    // Initialize wine config with defaults
    m_wineConfigWidget->setWineVersion("stable");
    m_wineConfigWidget->setWineDllOverrides("");
    m_wineConfigWidget->setWineArch("win64");
    
    // Move to wine config page
    m_stackedWidget->setCurrentIndex(2);
}

void MainWindow::configureWineSettings()
{
    // This method would be used to test the app in Wine before packaging
}

void MainWindow::generateFlatpakManifest()
{
    if (m_currentAppId.isEmpty() || !m_portableApps.contains(m_currentAppId)) {
        KMessageBox::error(this, i18n("No application selected!"), i18n("Error"));
        return;
    }
    
    PortableAppInfo &appInfo = m_portableApps[m_currentAppId];
    
    // Update the Wine settings from the config widget
    appInfo.wineVersion = m_wineConfigWidget->wineVersion();
    appInfo.wineDllOverrides = m_wineConfigWidget->wineDllOverrides();
    appInfo.wineArch = m_wineConfigWidget->wineArch();
    
    // Prepare manifest
    m_manifest.clear();
    m_manifest.setAppId("org.winepak." + appInfo.name.toLower().replace(" ", "_"));
    m_manifest.setAppName(appInfo.name);
    m_manifest.setAppVersion(appInfo.version);
    m_manifest.setAppDescription(appInfo.description);
    
    // Set icon if available
    if (!appInfo.iconPath.isEmpty() && QFile::exists(appInfo.iconPath)) {
        m_manifest.setAppIcon(appInfo.iconPath);
    }
    
    m_manifest.setRuntime("org.freedesktop.Platform");
    m_manifest.setRuntimeVersion("22.08");
    m_manifest.setSdk("org.freedesktop.Sdk");
    
    // Add Wine and related modules
    m_manifest.addWineModule(appInfo.wineVersion, appInfo.wineArch);
    
    // Configure environment variables
    QMap<QString, QString> env;
    env["WINEPREFIX"] = "/var/data/wine";
    
    if (!appInfo.wineDllOverrides.isEmpty()) {
        env["WINEDLLOVERRIDES"] = appInfo.wineDllOverrides;
    }
    
    m_manifest.setEnvironment(env);
    
    // Set the command to run
    QString relativeExePath = appInfo.executablePath;
    relativeExePath.replace(appInfo.sourceDir, "");
    if (relativeExePath.startsWith('/')) {
        relativeExePath.remove(0, 1);
    }
    
    m_manifest.setCommand("wine");
    m_manifest.addCommandArg("Z:\\app\\" + relativeExePath.replace("/", "\\"));
    
    // Configure filesystem access
    m_manifest.addFilesystemAccess("~/.local/share/winepak/" + m_manifest.appId() + ":create");
    m_manifest.addFilesystemAccess("xdg-documents");
    m_manifest.addFilesystemAccess("xdg-download");
    
    // Move to build page
    m_stackedWidget->setCurrentIndex(3);
    
    // Show manifest summary
    updateLog(i18n("Manifest generated for %1", appInfo.name));
    updateLog(i18n("App ID: %1", m_manifest.appId()));
    updateLog(i18n("Wine version: %1", appInfo.wineVersion));
    updateLog(i18n("Executable: %1", relativeExePath));
}

void MainWindow::buildFlatpak()
{
    if (m_currentAppId.isEmpty() || !m_portableApps.contains(m_currentAppId)) {
        KMessageBox::error(this, i18n("No application selected!"), i18n("Error"));
        return;
    }
    
    PortableAppInfo &appInfo = m_portableApps[m_currentAppId];
    
    // Prepare build directory
    QString buildDir = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) 
                     + "/flatpak-wine-builder/" + m_manifest.appId();
    QDir().mkpath(buildDir);
    
    // Write manifest to file
    QString manifestPath = buildDir + "/manifest.yml";
    if (!m_manifest.saveToFile(manifestPath)) {
        KMessageBox::error(this, i18n("Failed to write manifest file!"), i18n("Error"));
        return;
    }
    
    // Copy portable app to build directory
    QString appDestDir = buildDir + "/app";
    QDir().mkpath(appDestDir);
    
    // Using tar for copying to preserve symlinks and permissions
    QProcess copyProcess;
    copyProcess.start("cp", QStringList() << "-r" << appInfo.sourceDir + "/." << appDestDir);
    if (!copyProcess.waitForFinished() || copyProcess.exitCode() != 0) {
        KMessageBox::error(this, i18n("Failed to copy application files!"), i18n("Error"));
        return;
    }
    
    // Copy icon file if specified
    if (!appInfo.iconPath.isEmpty() && QFile::exists(appInfo.iconPath)) {
        QString iconDir = buildDir + "/icon";
        QDir().mkpath(iconDir);
        QFile::copy(appInfo.iconPath, iconDir + "/" + QFileInfo(appInfo.iconPath).fileName());
    }
    
    // Build the flatpak
    updateLog(i18n("Starting Flatpak build process..."));
    m_progressBar->setValue(10);
    
    // Set up build command
    m_process.setWorkingDirectory(buildDir);
    m_process.start("flatpak-builder", QStringList() 
                   << "--force-clean" 
                   << "--user" 
                   << "--install"
                   << "build" 
                   << manifestPath);
    
    // Disable the build button while building
    m_buildButton->setEnabled(false);
    updateLog(i18n("Building Flatpak... This may take several minutes."));
}

void MainWindow::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    m_buildButton->setEnabled(true);
    
    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        m_progressBar->setValue(100);
        updateLog(i18n("Flatpak built and installed successfully!"));
        KMessageBox::information(this, 
            i18n("The application has been packaged as a Flatpak and installed in your user repository."),
            i18n("Build Successful"));
    } else {
        updateLog(i18n("Flatpak build failed with exit code: %1", exitCode));
        KMessageBox::error(this, 
            i18n("Failed to build the Flatpak. Check the output log for details."),
            i18n("Build Failed"));
    }
}

void MainWindow::updateLog(const QString &message)
{
    m_statusLabel->setText(message);
    // In a real app, would also append to a log widget
}

void MainWindow::updateProgress(int value)
{
    m_progressBar->setValue(value);
}

void MainWindow::appSelected(int index)
{
    if (index < 0 || m_appsList->count() <= index)
        return;
    
    QString appName = m_appsList->item(index)->text();
    
    // Find the app ID from the name
    for (auto it = m_portableApps.constBegin(); it != m_portableApps.constEnd(); ++it) {
        if ((it.value().name + " (" + it.value().version + ")") == appName) {
            m_currentAppId = it.key();
            
            // Update the UI fields
            const PortableAppInfo &info = it.value();
            m_appNameEdit->setText(info.name);
            m_appVersionEdit->setText(info.version);
            m_appDescriptionEdit->setText(info.description);
            m_appCategoryEdit->setText(info.category);
            m_executablePathEdit->setText(info.executablePath);
            
            // Show the app details page
            m_stackedWidget->setCurrentIndex(1);
            return;
        }
    }
}

void MainWindow::removeSelectedApp()
{
    int currentRow = m_appsList->currentRow();
    if (currentRow < 0)
        return;
    
    QString appName = m_appsList->item(currentRow)->text();
    QString appId;
    
    // Find the app ID from the name
    for (auto it = m_portableApps.constBegin(); it != m_portableApps.constEnd(); ++it) {
        if ((it.value().name + " (" + it.value().version + ")") == appName) {
            appId = it.key();
            break;
        }
    }
    
    if (appId.isEmpty())
        return;
    
    // Ask for confirmation
    if (KMessageBox::questionYesNo(this,
            i18n("Are you sure you want to remove %1?", appName),
            i18n("Confirm Removal")) == KMessageBox::Yes) {
        
        // Remove from map and list
        m_portableApps.remove(appId);
        delete m_appsList->takeItem(currentRow);
        
        // If the removed app was the current one, reset
        if (m_currentAppId == appId) {
            m_currentAppId.clear();
            m_stackedWidget->setCurrentIndex(0);
        }
    }
}

void MainWindow::browseForIcon()
{
    QString iconPath = QFileDialog::getOpenFileName(this, 
                                                  i18n("Select Application Icon"),
                                                  QString(),
                                                  i18n("Icon Files (*.png *.svg *.jpg *.ico);;All Files (*)"));
    if (!iconPath.isEmpty()) {
        m_iconPathEdit->setText(iconPath);
        updateIconPreview(iconPath);
    }
}

void MainWindow::updateIconPreview(const QString &path)
{
    if (path.isEmpty() || !QFile::exists(path)) {
        m_iconPreviewLabel->clear();
        return;
    }
    
    QPixmap pixmap(path);
    if (!pixmap.isNull()) {
        pixmap = pixmap.scaled(48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        m_iconPreviewLabel->setPixmap(pixmap);
    } else {
        // Try to load using QIcon which can handle more formats
        QIcon icon(path);
        if (!icon.isNull()) {
            pixmap = icon.pixmap(48, 48);
            m_iconPreviewLabel->setPixmap(pixmap);
        } else {
            m_iconPreviewLabel->clear();
        }
    }
}

bool MainWindow::prepareWinePrefix(const PortableAppInfo &appInfo)
{
    // Create Wine prefix for testing
    QString prefixDir = m_tempDir.path() + "/wineprefix";
    QDir().mkpath(prefixDir);
    
    // Set environment for wine
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("WINEPREFIX", prefixDir);
    
    if (!appInfo.wineDllOverrides.isEmpty()) {
        env.insert("WINEDLLOVERRIDES", appInfo.wineDllOverrides);
    }
    
    // Initialize prefix (this will take a while first time)
    QProcess wineProcess;
    wineProcess.setProcessEnvironment(env);
    wineProcess.start("wineboot", QStringList() << "-i");
    
    if (!wineProcess.waitForFinished(30000)) { // 30 second timeout
        updateLog(i18n("Timeout waiting for Wine prefix initialization"));
        return false;
    }
    
    // Copy app to Wine drive_c
    QString appDir = prefixDir + "/drive_c/Program Files/PortableApp";
    QDir().mkpath(appDir);
    
    QProcess copyProcess;
    copyProcess.start("cp", QStringList() << "-r" << appInfo.sourceDir + "/." << appDir);
    
    if (!copyProcess.waitForFinished() || copyProcess.exitCode() != 0) {
        updateLog(i18n("Failed to copy application to Wine prefix"));
        return false;
    }
    
    return true;
}