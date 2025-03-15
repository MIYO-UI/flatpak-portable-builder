#ifndef PORTABLEAPPINFO_H
#define PORTABLEAPPINFO_H

#include <QString>
#include <QStringList>

/**
 * Class that holds information about a Windows PortableApp
 */
class PortableAppInfo {
public:
    // Unique identifier for this app
    QString id;
    
    // Application metadata
    QString name;
    QString version;
    QString description;
    QString category;
    
    // Paths
    QString sourceDir;      // Directory containing the PortableApp
    QString executablePath; // Path to the main executable
    
    // Wine configuration
    QString wineVersion;
    QString wineDllOverrides;
    QString wineArch;       // win32 or win64
    
    // Additional data
    QStringList requiredDLLs;
    QStringList additionalFiles;
    
    // Flatpak specific settings
    bool allowNetworkAccess = true;
    bool allowDocumentsAccess = true;
    bool allowDownloadsAccess = true;
    bool allowAudio = true;
    QString iconPath;
};

#endif // PORTABLEAPPINFO_H