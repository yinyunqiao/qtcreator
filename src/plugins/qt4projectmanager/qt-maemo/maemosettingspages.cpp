/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Assistant of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "maemosettingspages.h"

#include "maemoconstants.h"
#include "maemodeviceconfigurationssettingswidget.h"
#include "maemoqemusettings.h"
#include "maemoqemusettingswidget.h"

#include <coreplugin/icore.h>

#include <QtCore/QCoreApplication>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFrame>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>

namespace Qt4ProjectManager {
namespace Internal {

MaemoDeviceConfigurationsSettingsPage::MaemoDeviceConfigurationsSettingsPage(QObject *parent)
    : Core::IOptionsPage(parent)
{
}

MaemoDeviceConfigurationsSettingsPage::~MaemoDeviceConfigurationsSettingsPage()
{
}

QString MaemoDeviceConfigurationsSettingsPage::id() const
{
    return QLatin1String("ZZ.Maemo Device Configurations");
}

QString MaemoDeviceConfigurationsSettingsPage::displayName() const
{
    return tr("Maemo Device Configurations");
}

QString MaemoDeviceConfigurationsSettingsPage::category() const
{
    return QLatin1String(Constants::MAEMO_SETTINGS_CATEGORY);
}

QString MaemoDeviceConfigurationsSettingsPage::displayCategory() const
{
    return QCoreApplication::translate("Qt4ProjectManager",
        Constants::MAEMO_SETTINGS_TR_CATEGORY);
}

QIcon MaemoDeviceConfigurationsSettingsPage::categoryIcon() const
{
    return QIcon(QLatin1String(Constants::MAEMO_SETTINGS_CATEGORY_ICON));
}

bool MaemoDeviceConfigurationsSettingsPage::matches(const QString &searchKeyWord) const
{
    return m_keywords.contains(searchKeyWord, Qt::CaseInsensitive);
}

QWidget *MaemoDeviceConfigurationsSettingsPage::createPage(QWidget *parent)
{
    m_widget = new MaemoDeviceConfigurationsSettingsWidget(parent);
    if (m_keywords.isEmpty())
        m_keywords = m_widget->searchKeywords();
    return m_widget;
}

void MaemoDeviceConfigurationsSettingsPage::apply()
{
    m_widget->saveSettings();
}

void MaemoDeviceConfigurationsSettingsPage::finish()
{
}


MaemoQemuSettingsPage::MaemoQemuSettingsPage(QObject *parent)
    : Core::IOptionsPage(parent)
{
}

MaemoQemuSettingsPage::~MaemoQemuSettingsPage()
{
}

QString MaemoQemuSettingsPage::id() const
{
    return QLatin1String("ZZ.Qemu Settings");
}

QString MaemoQemuSettingsPage::displayName() const
{
    return tr("Qemu Settings");
}

QString MaemoQemuSettingsPage::category() const
{
    return QLatin1String(Constants::MAEMO_SETTINGS_CATEGORY);
}

QString MaemoQemuSettingsPage::displayCategory() const
{
    return QCoreApplication::translate("Qt4ProjectManager",
        Constants::MAEMO_SETTINGS_TR_CATEGORY);
}

QIcon MaemoQemuSettingsPage::categoryIcon() const
{
    return QIcon(QLatin1String(Constants::MAEMO_SETTINGS_CATEGORY_ICON));
}

bool MaemoQemuSettingsPage::matches(const QString &searchKeyWord) const
{
    return m_widget->keywords().contains(searchKeyWord, Qt::CaseInsensitive);
}

QWidget *MaemoQemuSettingsPage::createPage(QWidget *parent)
{
    m_widget = new MaemoQemuSettingsWidget(parent);
    return m_widget;
}

void MaemoQemuSettingsPage::apply()
{
    m_widget->saveSettings();
}

void MaemoQemuSettingsPage::finish()
{
}


class MaemoQemuCrashDialog : public QDialog
{
    Q_OBJECT
public:
    MaemoQemuCrashDialog(MaemoQemuSettingsPage *settingsPage)
        : m_settingsPage(settingsPage)
    {
        setWindowTitle(tr("Qemu error"));
        QString message = tr("Qemu crashed.");
        const MaemoQemuSettings::OpenGlMode openGlMode
            = MaemoQemuSettings::openGlMode();
        const QString linkString = QLatin1String("<p><a href=\"dummy\">")
            + tr("Click here to change the OpenGL mode.")
            + QLatin1String("</a>");
        if (openGlMode == MaemoQemuSettings::HardwareAcceleration) {
            message += tr("<p>You have configured Qemu to use OpenGL "
                "hardware acceleration, which might not be supported by "
                "your system. You could try using software rendering instead.");
            message += linkString;
        } else if (openGlMode == MaemoQemuSettings::AutoDetect) {
            message += tr("<p>Qemu is currently configured to auto-detect the "
                "OpenGL mode, which is known to not work in some cases."
                "You might want to use software rendering instead.");
            message += linkString;
        }
        QLabel * const messageLabel = new QLabel(message, this);
        messageLabel->setWordWrap(true);
        messageLabel->setTextFormat(Qt::RichText);
        connect(messageLabel, SIGNAL(linkActivated(QString)),
            SLOT(showSettingsPage()));
        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        mainLayout->addWidget(messageLabel);
        QFrame * const separator = new QFrame;
        separator->setFrameShape(QFrame::HLine);
        separator->setFrameShadow(QFrame::Sunken);
        mainLayout->addWidget(separator);
        QDialogButtonBox * const buttonBox = new QDialogButtonBox;
        buttonBox->addButton(QDialogButtonBox::Ok);
        connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
        mainLayout->addWidget(buttonBox);
    }

private:
    Q_SLOT void showSettingsPage()
    {
        Core::ICore::instance()->showOptionsDialog(m_settingsPage->category(),
            m_settingsPage->id());
        accept();
    }

    MaemoQemuSettingsPage * const m_settingsPage;
};


void MaemoQemuSettingsPage::showQemuCrashDialog()
{
    MaemoQemuCrashDialog dlg(this);
    dlg.exec();
}

} // namespace Internal
} // namespace Qt4ProjectManager

#include "maemosettingspages.moc"
