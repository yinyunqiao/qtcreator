/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "nodeinstancemetaobject.h"

#include "objectnodeinstance.h"
#include <QSharedPointer>
#include <QMetaProperty>
#include <qnumeric.h>
#include <QtDebug>

namespace QmlDesigner {
namespace Internal {

NodeInstanceMetaObject::NodeInstanceMetaObject(const ObjectNodeInstance::Pointer &nodeInstance, QDeclarativeEngine *engine)
    : QDeclarativeOpenMetaObject(nodeInstance->object(), new QDeclarativeOpenMetaObjectType(nodeInstance->object()->metaObject(), engine), true),
    m_nodeInstance(nodeInstance),
    m_context(nodeInstance->isRootNodeInstance() ? nodeInstance->context() : 0)
{
    setCached(true);
}

NodeInstanceMetaObject::NodeInstanceMetaObject(const ObjectNodeInstancePointer &nodeInstance, QObject *object, const QString &prefix, QDeclarativeEngine *engine)
    : QDeclarativeOpenMetaObject(object, new QDeclarativeOpenMetaObjectType(object->metaObject(), engine), true),
    m_nodeInstance(nodeInstance),
    m_prefix(prefix)
{
    setCached(true);
}

void NodeInstanceMetaObject::createNewProperty(const QString &name)
{
    int id = createProperty(name.toLatin1(), 0);
    Q_ASSERT(id >= 0);
}

int NodeInstanceMetaObject::metaCall(QMetaObject::Call call, int id, void **a)
{
    int metaCallReturnValue = -1;

    if (call == QMetaObject::WriteProperty
        && property(id).userType() == QMetaType::QVariant
        && reinterpret_cast<QVariant *>(a[0])->type() == QVariant::Double
        && qIsNaN(reinterpret_cast<QVariant *>(a[0])->toDouble())) {
        return -1;
    }

    if (call == QMetaObject::WriteProperty
        && property(id).userType() == QMetaType::Double
        && qIsNaN(*reinterpret_cast<double*>(a[0]))) {
        return -1;
    }

    if (call == QMetaObject::WriteProperty
        && property(id).userType() == QMetaType::Float
        && qIsNaN(*reinterpret_cast<float*>(a[0]))) {
        return -1;
    }

    QVariant oldValue;

    if (call == QMetaObject::WriteProperty && !property(id).hasNotifySignal())
    {
        oldValue = property(id).read(m_nodeInstance->object());
    }

    ObjectNodeInstance::Pointer objectNodeInstance = m_nodeInstance.toStrongRef();

    if (parent() && id < parent()->propertyOffset()) {
        metaCallReturnValue = parent()->metaCall(call, id, a);
    } else {
        metaCallReturnValue = QDeclarativeOpenMetaObject::metaCall(call, id, a);
    }

    if ((call == QMetaObject::WriteProperty || call == QMetaObject::ReadProperty) && metaCallReturnValue < 0) {
        if (objectNodeInstance
                && objectNodeInstance->nodeInstanceServer()
                && objectNodeInstance->nodeInstanceServer()->dummyContextObject()
                && !(objectNodeInstance && !objectNodeInstance->isRootNodeInstance() && property(id).name() == QLatin1String("parent"))) {

            QObject *contextDummyObject = objectNodeInstance->nodeInstanceServer()->dummyContextObject();
            int properyIndex = contextDummyObject->metaObject()->indexOfProperty(property(id).name());
            if (properyIndex >= 0)
                metaCallReturnValue = contextDummyObject->qt_metacall(call, properyIndex, a);
        }
    }

    if (metaCallReturnValue >= 0
            && call == QMetaObject::WriteProperty
            && !property(id).hasNotifySignal()
            && oldValue != property(id).read(m_nodeInstance->object()))
        notifyPropertyChange(id);

    return metaCallReturnValue;
}

void NodeInstanceMetaObject::notifyPropertyChange(int id)
{
    ObjectNodeInstance::Pointer objectNodeInstance = m_nodeInstance.toStrongRef();

    if (objectNodeInstance && objectNodeInstance->nodeInstanceServer()) {
        if (id < type()->propertyOffset()) {
            objectNodeInstance->nodeInstanceServer()->notifyPropertyChange(objectNodeInstance->instanceId(), m_prefix + property(id).name());
        } else {
            objectNodeInstance->nodeInstanceServer()->notifyPropertyChange(objectNodeInstance->instanceId(), m_prefix + name(id - type()->propertyOffset()));
        }
    }
}

} // namespace Internal
} // namespace QmlDesigner
