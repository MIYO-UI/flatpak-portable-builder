#include "flatpakmanifest.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

FlatpakManifest::FlatpakManifest()
    : m_allowNetwork(true)
    , m_allowAudio(true)
{
    clear();
}

void FlatpakManifest::clear()
{
    m_appId.clear();
    m_appName.clear();
    m_appVersion.clear();
    m_appDescription.clear();
    m_appIcon.clear();
    
    m_runtime = "org.freedesktop.Platform";
    m_runtimeVersion = "22.08";
    m_sdk = "org.freedesktop.Sdk";
    
    m_command.clear();
    m_commandArgs.clear();
    m_environment.clear();
    
    m_filesystemAccess.clear();
    
    m_allowNetwork = true;
    m_allowAudio = true;
    
    m_modules.empty();
    m_extensions.clear();
}

void FlatpakManifest::setAppId(const QString &appId)
{
    m_appId = appId;
}

void FlatpakManifest::setAppName(const QString &name)
{
    m_appName = name;
}

void FlatpakManifest::setAppVersion(const QString &version)
{
    m_appVersion = version;
}

void FlatpakManifest::setAppDescription(const QString &description)
{
    m_appDescription = description;
}

void FlatpakManifest::setAppIcon(const QString &iconPath)
{
    m_appIcon = iconPath;
}

void FlatpakManifest::setRuntime(const QString &runtime)
{
    m_runtime = runtime;
}

void FlatpakManifest::setRuntimeVersion(const QString &version)
{
    m_runtimeVersion = version;
}

void FlatpakManifest::setSdk(const QString &sdk)
{
    m_sdk = sdk;
}

void FlatpakManifest::addWineModule(const QString &wineVersion, const QString &arch)
{
    // Create Wine module
    QJsonObject wineModule;
    wineModule["name"] = "wine";
    wineModule["buildsystem"] = "simple";
    
    // Configure Wine build commands
    QJsonArray buildCommands;
    
    // Install Wine from the repository
    QJsonObject installCommand;
    installCommand["type"] = "shell";
    
    QString command = "mkdir -p ${FLATPAK_DEST}/wine";
    
    // Determine which version of Wine to install
    if (wineVersion == "staging") {
        command += " && dnf install -y --installroot=${FLATPAK_DEST}/wine wine-staging";
    } else if (wineVersion == "devel") {
        command += " && dnf install -y --installroot=${FLATPAK_DEST}/wine wine-devel";
    } else {
        // Default to stable Wine
        command += " && dnf install -y --installroot=${FLATPAK_DEST}/wine wine";
    }
    
    installCommand["commands"] = QJsonArray{command};
    buildCommands.append(installCommand);
    
    wineModule["build-commands"] = buildCommands;
    
    // Add to modules
    m_modules.append(wineModule);
    
    // Add Wine app module (containing the actual Windows app)
    QJsonObject appModule;
    appModule["name"] = "app";
    appModule["buildsystem"] = "simple";
    
    QJsonArray appBuildCommands;
    QJsonObject appInstallCommand;
    appInstallCommand["type"] = "shell";
    appInstallCommand["commands"] = QJsonArray{
        "mkdir -p ${FLATPAK_DEST}/app",
        "cp -r * ${FLATPAK_DEST}/app/"
    };
    appBuildCommands.append(appInstallCommand);
    
    appModule["build-commands"] = appBuildCommands;
    m_modules.append(appModule);
}

void FlatpakManifest::addDxvkModule(const QString &dxvkVersion)
{
    // Create DXVK module for DirectX to Vulkan translation
    QJsonObject dxvkModule;
    dxvkModule["name"] = "dxvk";
    dxvkModule["buildsystem"] = "simple";
    
    QJsonArray buildCommands;
    QJsonObject installCommand;
    installCommand["type"] = "shell";
    
    QString command = "mkdir -p ${FLATPAK_DEST}/dxvk";
    
    // Determine DXVK version to download
    QString dxvkUrl;
    
    if (dxvkVersion == "latest") {
        dxvkUrl = "https://github.com/doitsujin/dxvk/releases/latest/download/dxvk-latest.tar.gz";
    } else {
        dxvkUrl = "https://github.com/doitsujin/dxvk/releases/download/v" + dxvkVersion + "/dxvk-" + dxvkVersion + ".tar.gz";
    }
    
    command += " && curl -L " + dxvkUrl + " -o dxvk.tar.gz";
    command += " && tar -xf dxvk.tar.gz -C ${FLATPAK_DEST}/dxvk --strip-components=1";
    
    installCommand["commands"] = QJsonArray{command};
    buildCommands.append(installCommand);
    
    dxvkModule["build-commands"] = buildCommands;
    
    // Add to modules
    m_modules.append(dxvkModule);
}

void FlatpakManifest::setCommand(const QString &command)
{
    m_command = command;
}

void FlatpakManifest::addCommandArg(const QString &arg)
{
    m_commandArgs.append(arg);
}

void FlatpakManifest::setEnvironment(const QMap<QString, QString> &env)
{
    m_environment = env;
}

void FlatpakManifest::addFilesystemAccess(const QString &path)
{
    m_filesystemAccess.append(path);
}

void FlatpakManifest::setAllowNetwork(bool allow)
{
    m_allowNetwork = allow;
}

void FlatpakManifest::setAllowAudio(bool allow)
{
    m_allowAudio = allow;
}

void FlatpakManifest::addModule(const QJsonObject &module)
{
    m_modules.append(module);
}

void FlatpakManifest::addExtension(const QString &extensionName)
{
    m_extensions.append(extensionName);
}

bool FlatpakManifest::saveToFile(const QString &filePath) const
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;
    
    QJsonDocument doc(toJsonObject());
    file.write(doc.toJson());
    
    return true;
}

QJsonObject FlatpakManifest::toJsonObject() const
{
    QJsonObject manifest;
    
    // Basic app information
    manifest["app-id"] = m_appId;
    manifest["runtime"] = m_runtime;
    manifest["runtime-version"] = m_runtimeVersion;
    manifest["sdk"] = m_sdk;
    
    // Command
    manifest["command"] = m_command;
    
    if (!m_commandArgs.isEmpty()) {
        QJsonArray args;
        for (const QString &arg : m_commandArgs) {
            args.append(arg);
        }
        manifest["command-args"] = args;
    }
    
    // Finish args (configure sandbox)
    QJsonArray finishArgs;
    
    // Always share IPC namespace
    finishArgs.append("--share=ipc");
    
    // Network access
    if (m_allowNetwork) {
        finishArgs.append("--share=network");
    } else {
        finishArgs.append("--unshare=network");
    }
    
    // Audio access
    if (m_allowAudio) {
        finishArgs.append("--socket=pulseaudio");
    }
    
    // X11 access for Wine
    finishArgs.append("--socket=x11");
    finishArgs.append("--share=network");
    
    // DRI (hardware acceleration)
    finishArgs.append("--device=dri");
    
    // Filesystem access
    for (const QString &path : m_filesystemAccess) {
        finishArgs.append("--filesystem=" + path);
    }
    
    manifest["finish-args"] = finishArgs;
    
    // Add environment if specified
    if (!m_environment.isEmpty()) {
        QJsonObject env;
        for (auto it = m_environment.constBegin(); it != m_environment.constEnd(); ++it) {
            env[it.key()] = it.value();
        }
        manifest["environment"] = env;
    }
    
    // Add metadata
    QJsonObject metadata;
    
    if (!m_appName.isEmpty()) {
        metadata["name"] = m_appName;
    }
    
    if (!m_appDescription.isEmpty()) {
        QJsonObject comments;
        comments["C"] = m_appDescription;
        metadata["comments"] = comments;
    }
    
    if (!metadata.isEmpty()) {
        manifest["metadata"] = metadata;
    }
    
    // Add modules
    if (!m_modules.isEmpty()) {
        manifest["modules"] = m_modules;
    }
    
    return manifest;
}