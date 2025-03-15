#ifndef FLATPAKMANIFEST_H
#define FLATPAKMANIFEST_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <QJsonObject>
#include <QJsonArray>

/**
 * Class to generate Flatpak manifest files for Wine applications
 */
class FlatpakManifest
{
public:
    FlatpakManifest();
    
    // Clear the manifest
    void clear();
    
    // Basic metadata
    void setAppId(const QString &appId);
    void setAppName(const QString &name);
    void setAppVersion(const QString &version);
    void setAppDescription(const QString &description);
    void setAppIcon(const QString &iconPath);
    
    // Runtime settings
    void setRuntime(const QString &runtime);
    void setRuntimeVersion(const QString &version);
    void setSdk(const QString &sdk);
    
    // Wine settings
    void addWineModule(const QString &wineVersion, const QString &arch);
    void addDxvkModule(const QString &dxvkVersion = "latest");
    
    // Command configuration
    void setCommand(const QString &command);
    void addCommandArg(const QString &arg);
    void setEnvironment(const QMap<QString, QString> &env);
    
    // Filesystem access
    void addFilesystemAccess(const QString &path);
    
    // Network access
    void setAllowNetwork(bool allow);
    
    // Other permissions
    void setAllowAudio(bool allow);
    
    // Modules and extensions
    void addModule(const QJsonObject &module);
    void addExtension(const QString &extensionName);
    
    // Save to file
    bool saveToFile(const QString &filePath) const;
    
    // Get the generated manifest
    QJsonObject toJsonObject() const;
    
    // Access some of the set values
    QString appId() const { return m_appId; }
    QString appName() const { return m_appName; }
    QString appVersion() const { return m_appVersion; }
    
private:
    // Basic metadata
    QString m_appId;
    QString m_appName;
    QString m_appVersion;
    QString m_appDescription;
    QString m_appIcon;
    
    // Runtime settings
    QString m_runtime;
    QString m_runtimeVersion;
    QString m_sdk;
    
    // Command configuration
    QString m_command;
    QStringList m_commandArgs;
    QMap<QString, QString> m_environment;
    
    // Filesystem access
    QStringList m_filesystemAccess;
    
    // Other permissions
    bool m_allowNetwork;
    bool m_allowAudio;
    
    // Modules and extensions
    QJsonArray m_modules;
    QStringList m_extensions;
};

#endif // FLATPAKMANIFEST_H