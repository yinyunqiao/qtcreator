/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QMLDEBUGGINGLIBRARY_H
#define QMLDEBUGGINGLIBRARY_H

#include <utils/buildablehelperlibrary.h>
#include "qt4projectmanager_global.h"

QT_FORWARD_DECLARE_CLASS(QDir)

namespace Utils {
    class Environment;
}

namespace ProjectExplorer {
    class Project;
}

namespace Qt4ProjectManager {

class QtVersion;

class QmlDebuggingLibrary : public Utils::BuildableHelperLibrary
{
public:
    static QString libraryByInstallData(const QString &qtInstallData, bool debugBuild);

    static bool canBuild(const QtVersion *qtVersion);
    static bool build(const QString &directory, const QString &makeCommand,
                       const QString &qmakeCommand, const QString &mkspec,
                       const Utils::Environment &env, const QString &targetMode,
                       const QStringList &qmakeArguments, QString *output,  QString *errorMessage);
    static QString copy(const QString &qtInstallData, QString *errorMessage);

private:
    static QStringList recursiveFileList(const QDir &dir, const QString &prefix = QString());
    static QStringList installDirectories(const QString &qtInstallData);
    static QString sourcePath();
    static QStringList sourceFileNames();
};

} // namespace

#endif // QMLDEBUGGINGLIBRARY_H
