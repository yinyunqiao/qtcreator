/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2011 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** No Commercial Usage
**
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
**
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
**************************************************************************/

#ifndef S60EMULATORRUNCONFIGURATION_H
#define S60EMULATORRUNCONFIGURATION_H

#include <projectexplorer/runconfiguration.h>
#include <projectexplorer/applicationlauncher.h>

#include <QtCore/QVariantMap>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE
class QLabel;
class QLineEdit;
QT_END_NAMESPACE

namespace Utils {
class DetailsWidget;
}

namespace Qt4ProjectManager {
class Qt4Project;
class Qt4BaseTarget;

namespace Internal {
class Qt4SymbianTarget;
class Qt4ProFileNode;
class S60EmulatorRunConfigurationFactory;

class S60EmulatorRunConfiguration : public ProjectExplorer::RunConfiguration
{
    Q_OBJECT
    friend class S60EmulatorRunConfigurationFactory;

public:
    S60EmulatorRunConfiguration(Qt4ProjectManager::Qt4BaseTarget *parent, const QString &proFilePath);
    virtual ~S60EmulatorRunConfiguration();

    Qt4SymbianTarget *qt4Target() const;

    using ProjectExplorer::RunConfiguration::isEnabled;
    bool isEnabled(ProjectExplorer::BuildConfiguration *configuration) const;
    QWidget *createConfigurationWidget();

    ProjectExplorer::OutputFormatter *createOutputFormatter() const;

    QString executable() const;

    QVariantMap toMap() const;

    QString proFilePath() const;

signals:
    void targetInformationChanged();

private slots:
    void proFileUpdate(Qt4ProjectManager::Internal::Qt4ProFileNode *pro, bool success);
    void proFileInvalidated(Qt4ProjectManager::Internal::Qt4ProFileNode *pro);

protected:
    S60EmulatorRunConfiguration(Qt4ProjectManager::Qt4BaseTarget *parent, S60EmulatorRunConfiguration *source);
    virtual bool fromMap(const QVariantMap &map);

private:
    void ctor();
    void handleParserState(bool sucess);
    void updateTarget();

    QString m_proFilePath;
    bool m_validParse;
};

class S60EmulatorRunConfigurationWidget : public QWidget
{
    Q_OBJECT
public:
    explicit S60EmulatorRunConfigurationWidget(S60EmulatorRunConfiguration *runConfiguration,
                                               QWidget *parent = 0);

private slots:
    void updateTargetInformation();
    void runConfigurationEnabledChange(bool enabled);

private:
    S60EmulatorRunConfiguration *m_runConfiguration;
    Utils::DetailsWidget *m_detailsWidget;
    QLabel *m_executableLabel;
};

class S60EmulatorRunConfigurationFactory : public ProjectExplorer::IRunConfigurationFactory
{
    Q_OBJECT
public:
    explicit S60EmulatorRunConfigurationFactory(QObject *parent = 0);
    ~S60EmulatorRunConfigurationFactory();

    bool canCreate(ProjectExplorer::Target *project, const QString &id) const;
    ProjectExplorer::RunConfiguration *create(ProjectExplorer::Target *parent, const QString &id);
    bool canRestore(ProjectExplorer::Target *parent, const QVariantMap &map) const;
    ProjectExplorer::RunConfiguration *restore(ProjectExplorer::Target *parent, const QVariantMap &map);
    bool canClone(ProjectExplorer::Target *parent, ProjectExplorer::RunConfiguration *source) const;
    ProjectExplorer::RunConfiguration *clone(ProjectExplorer::Target *parent, ProjectExplorer::RunConfiguration *source);

    QStringList availableCreationIds(ProjectExplorer::Target *pro) const;
    // used to translate the ids to names to display to the user
    QString displayNameForId(const QString &id) const;
};

class S60EmulatorRunControl : public ProjectExplorer::RunControl
{
    Q_OBJECT
public:
    explicit S60EmulatorRunControl(S60EmulatorRunConfiguration *runConfiguration, QString mode);
    ~S60EmulatorRunControl() {}
    void start();
    virtual StopResult stop();
    bool isRunning() const;

private slots:
    void processExited(int exitCode);
    void slotAppendMessage(const QString &line, ProjectExplorer::OutputFormat);
    void slotError(const QString & error);

private:
    ProjectExplorer::ApplicationLauncher m_applicationLauncher;
    QString m_executable;
};

} // namespace Internal
} // namespace Qt4ProjectManager

#endif // S60EMULATORRUNCONFIGURATION_H
