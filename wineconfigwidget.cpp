#include "wineconfigwidget.h"

#include <QComboBox>
#include <QLineEdit>
#include <QGroupBox>
#include <QCheckBox>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <KLocalizedString>

WineConfigWidget::WineConfigWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
}

void WineConfigWidget::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Wine Configuration Group
    QGroupBox *wineGroup = new QGroupBox(i18n("Wine Configuration"));
    QFormLayout *wineLayout = new QFormLayout(wineGroup);
    
    m_wineVersionCombo = new QComboBox();
    m_wineVersionCombo->addItem(i18n("Stable"), "stable");
    m_wineVersionCombo->addItem(i18n("Development"), "devel");
    m_wineVersionCombo->addItem(i18n("Staging"), "staging");
    
    m_wineArchCombo = new QComboBox();
    m_wineArchCombo->addItem(i18n("64-bit (win64)"), "win64");
    m_wineArchCombo->addItem(i18n("32-bit (win32)"), "win32");
    
    m_wineDllOverridesEdit = new QLineEdit();
    m_wineDllOverridesEdit->setPlaceholderText(i18n("e.g., mscoree=n,b"));
    
    wineLayout->addRow(i18n("Wine Version:"), m_wineVersionCombo);
    wineLayout->addRow(i18n("Architecture:"), m_wineArchCombo);
    wineLayout->addRow(i18n("DLL Overrides:"), m_wineDllOverridesEdit);
    
    // DXVK Configuration Group
    m_dxvkGroup = new QGroupBox(i18n("DXVK Configuration (DirectX to Vulkan)"));
    QVBoxLayout *dxvkLayout = new QVBoxLayout(m_dxvkGroup);
    
    m_enableDxvkCheck = new QCheckBox(i18n("Enable DXVK"));
    
    QHBoxLayout *dxvkVersionLayout = new QHBoxLayout();
    QLabel *dxvkVersionLabel = new QLabel(i18n("DXVK Version:"));
    m_dxvkVersionCombo = new QComboBox();
    m_dxvkVersionCombo->addItem(i18n("Latest"), "latest");
    m_dxvkVersionCombo->addItem(i18n("2.2"), "2.2");
    m_dxvkVersionCombo->addItem(i18n("2.1"), "2.1");
    m_dxvkVersionCombo->addItem(i18n("2.0"), "2.0");
    m_dxvkVersionCombo->setEnabled(false);
    
    dxvkVersionLayout->addWidget(dxvkVersionLabel);
    dxvkVersionLayout->addWidget(m_dxvkVersionCombo);
    
    dxvkLayout->addWidget(m_enableDxvkCheck);
    dxvkLayout->addLayout(dxvkVersionLayout);
    
    // Connect DXVK checkbox to enable/disable version combo
    connect(m_enableDxvkCheck, &QCheckBox::toggled, m_dxvkVersionCombo, &QComboBox::setEnabled);
    
    // Permissions Group
    m_permissionsGroup = new QGroupBox(i18n("Flatpak Permissions"));
    QVBoxLayout *permissionsLayout = new QVBoxLayout(m_permissionsGroup);
    
    m_networkCheck = new QCheckBox(i18n("Allow Network Access"));
    m_documentsCheck = new QCheckBox(i18n("Allow Documents Folder Access"));
    m_downloadsCheck = new QCheckBox(i18n("Allow Downloads Folder Access"));
    m_audioCheck = new QCheckBox(i18n("Allow Audio"));
    
    m_networkCheck->setChecked(true);
    m_documentsCheck->setChecked(true);
    m_downloadsCheck->setChecked(true);
    m_audioCheck->setChecked(true);
    
    permissionsLayout->addWidget(m_networkCheck);
    permissionsLayout->addWidget(m_documentsCheck);
    permissionsLayout->addWidget(m_downloadsCheck);
    permissionsLayout->addWidget(m_audioCheck);
    
    // Add all groups to main layout
    mainLayout->addWidget(wineGroup);
    mainLayout->addWidget(m_dxvkGroup);
    mainLayout->addWidget(m_permissionsGroup);
    mainLayout->addStretch();
}

QString WineConfigWidget::wineVersion() const
{
    return m_wineVersionCombo->currentData().toString();
}

QString WineConfigWidget::wineDllOverrides() const
{
    return m_wineDllOverridesEdit->text();
}

QString WineConfigWidget::wineArch() const
{
    return m_wineArchCombo->currentData().toString();
}

void WineConfigWidget::setWineVersion(const QString &version)
{
    for (int i = 0; i < m_wineVersionCombo->count(); ++i) {
        if (m_wineVersionCombo->itemData(i).toString() == version) {
            m_wineVersionCombo->setCurrentIndex(i);
            break;
        }
    }
}

void WineConfigWidget::setWineDllOverrides(const QString &overrides)
{
    m_wineDllOverridesEdit->setText(overrides);
}

void WineConfigWidget::setWineArch(const QString &arch)
{
    for (int i = 0; i < m_wineArchCombo->count(); ++i) {
        if (m_wineArchCombo->itemData(i).toString() == arch) {
            m_wineArchCombo->setCurrentIndex(i);
            break;
        }
    }
}