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

#ifndef RIGHTPANE_H
#define RIGHTPANE_H

#include "core_global.h"

#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE
class QSettings;
QT_END_NAMESPACE

namespace Core {

class IMode;
class RightPaneWidget;

// TODO: The right pane works only for the help plugin atm.  It can't cope
// with more than one plugin publishing objects they want in the right pane
// For that the API would need to be different. (Might be that instead of
// adding objects to the pool, there should be a method
// RightPaneWidget::setWidget(QWidget *w) Anyway if a second plugin wants to
// show something there, redesign this API

class CORE_EXPORT RightPanePlaceHolder : public QWidget
{
    friend class Core::RightPaneWidget;
    Q_OBJECT

public:
    explicit RightPanePlaceHolder(Core::IMode *mode, QWidget *parent = 0);
    ~RightPanePlaceHolder();
    static RightPanePlaceHolder *current();

private slots:
    void currentModeChanged(Core::IMode *);

private:
    void applyStoredSize(int width);
    Core::IMode *m_mode;
    static RightPanePlaceHolder* m_current;
};


class CORE_EXPORT BaseRightPaneWidget : public QObject
{
    Q_OBJECT

public:
    BaseRightPaneWidget(QWidget *widget);
    ~BaseRightPaneWidget();
    QWidget *widget() const;

private:
    QWidget *m_widget;
};


class CORE_EXPORT RightPaneWidget : public QWidget
{
    Q_OBJECT

public:
    RightPaneWidget();
    ~RightPaneWidget();

    void saveSettings(QSettings *settings);
    void readSettings(QSettings *settings);

    bool isShown();
    void setShown(bool b);

    static RightPaneWidget *instance();

    int storedWidth();

protected:
    void resizeEvent(QResizeEvent *);

private slots:
    void objectAdded(QObject *obj);
    void aboutToRemoveObject(QObject *obj);

private:
    bool m_shown;
    int m_width;
    static RightPaneWidget *m_instance;
};

} // namespace Core

#endif // RIGHTPANE_H
