#ifndef WINECONFIGWIDGET_H
#define WINECONFIGWIDGET_H

#include <QWidget>

class QComboBox;
class QLineEdit;
class QGroupBox;
class QCheckBox;

/**
 * Widget for configuring Wine settings
 */
class WineConfigWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit WineConfigWidget(QWidget *parent = nullptr);
    
    // Getters
    QString wineVersion() const;
    QString wineDllOverrides() const;
    QString wineArch() const;
    
    // Setters
    void setWineVersion(const QString &version);
    void setWineDllOverrides(const QString &overrides);
    void setWineArch(const QString &arch);
    
private:
    void setupUi();
    
    QComboBox *m_wineVersionCombo;
    QLineEdit *m_wineDllOverridesEdit;
    QComboBox *m_wineArchCombo;
    
    QGroupBox *m_dxvkGroup;
    QCheckBox *m_enableDxvkCheck;
    QComboBox *m_dxvkVersionCombo;
    
    QGroupBox *m_permissionsGroup;
    QCheckBox *m_networkCheck;
    QCheckBox *m_documentsCheck;
    QCheckBox *m_downloadsCheck;
    QCheckBox *m_audioCheck;
};

#endif // WINECONFIGWIDGET_H