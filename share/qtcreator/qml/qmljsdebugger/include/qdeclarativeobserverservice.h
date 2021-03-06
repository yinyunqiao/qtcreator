/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#ifndef QDECLARATIVEDESIGNDEBUGSERVER_H
#define QDECLARATIVEDESIGNDEBUGSERVER_H

#include "qt_private/qdeclarativedebugservice_p.h"
#include "qmlobserverconstants.h"
#include "qmljsdebugger_global.h"

#include <QtCore/QHash>

QT_FORWARD_DECLARE_CLASS(QColor)
QT_FORWARD_DECLARE_CLASS(QDeclarativeEngine)
QT_FORWARD_DECLARE_CLASS(QDeclarativeContext)
QT_FORWARD_DECLARE_CLASS(QDeclarativeWatcher)
QT_FORWARD_DECLARE_CLASS(QDataStream)

namespace QmlJSDebugger {

class QMLJSDEBUGGER_EXPORT QDeclarativeObserverService : public QDeclarativeDebugService
{
    Q_OBJECT
public:
    QDeclarativeObserverService();
    static QDeclarativeObserverService *instance();

    void setDesignModeBehavior(bool inDesignMode);
    void setCurrentObjects(QList<QObject*> items);
    void setAnimationSpeed(qreal slowdownFactor);
    void setCurrentTool(QmlJSDebugger::Constants::DesignTool toolId);
    void reloaded();
    void setShowAppOnTop(bool showAppOnTop);

    QString idStringForObject(QObject *obj) const;

    void sendMessage(const QByteArray &message);

public Q_SLOTS:
    void selectedColorChanged(const QColor &color);
    void contextPathUpdated(const QStringList &contextPath);

Q_SIGNALS:
    void debuggingClientChanged(bool hasDebuggingClient);

    void currentObjectsChanged(const QList<QObject*> &objects);
    void designModeBehaviorChanged(bool inDesignMode);
    void showAppOnTopChanged(bool showAppOnTop);
    void reloadRequested();
    void selectToolRequested();
    void selectMarqueeToolRequested();
    void zoomToolRequested();
    void colorPickerToolRequested();

    void objectCreationRequested(const QString &qml, QObject *parent,
                                 const QStringList &imports, const QString &filename = QString());
    void objectReparentRequested(QObject *object, QObject *newParent);

    // 1 = normal speed,
    // 0 = paused,
    // 1 < x < 16 = slowdown by some factor
    void animationSpeedChangeRequested(qreal speedFactor);

    void contextPathIndexChanged(int contextPathIndex);
    void clearComponentCacheRequested();

protected:
    virtual void statusChanged(Status status);
    virtual void messageReceived(const QByteArray &);

private:
    QHash<int, QString> m_stringIdForObjectId;
};

} // namespace QmlJSDebugger

#endif // QDECLARATIVEDESIGNDEBUGSERVER_H
