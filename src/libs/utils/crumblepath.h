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

#ifndef CRUMBLEPATH_H
#define CRUMBLEPATH_H

#include "utils_global.h"

#include <QtGui/QWidget>
#include <QtCore/QVariant>

QT_FORWARD_DECLARE_CLASS(QResizeEvent)

namespace Utils {

struct CrumblePathPrivate;

class QTCREATOR_UTILS_EXPORT CrumblePath : public QWidget
{
    Q_OBJECT
public:
    explicit CrumblePath(QWidget *parent = 0);
    ~CrumblePath();
    void selectIndex(int index);
    QVariant dataForIndex(int index) const;

public slots:
    void pushElement(const QString &title, const QVariant &data = QVariant());
    void addChild(const QString &title, const QVariant &data = QVariant());
    void popElement();
    void clear();

signals:
    void elementClicked(int debugId);

protected:
    void resizeEvent(QResizeEvent *);

private slots:
    void mapClickToIndex();

private:
    void resizeButtons();
    void setBackgroundStyle();

private:
    QScopedPointer<CrumblePathPrivate> d;
};

} // namespace Utils

#endif // CRUMBLEPATH_H
