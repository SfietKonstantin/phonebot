/*
 * Copyright (C) 2014 Lucien XU <sfietkonstantin@free.fr>
 *
 * You may use this file under the terms of the BSD license as follows:
 *
 * "Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * The names of its contributors may not be used to endorse or promote
 *     products derived from this software without specific prior written
 *     permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
 */

#include "metacomponent.h"
#include "abstractmetadata.h"
#include <QtCore/QDebug>
#include <QtCore/QMetaClassInfo>
#include <QtCore/QMetaType>
#include <QtQml/private/qqmlmetatype_p.h>

struct MetaComponentPrivate
{
    MetaComponentPrivate();
    QStringList properties;
    AbstractMetaData *metaData;
};

MetaComponentPrivate::MetaComponentPrivate()
    : metaData(0)
{
}

MetaComponent::MetaComponent(QObject *parent) :
    QObject(parent), d_ptr(new MetaComponentPrivate)
{
}

MetaComponent::~MetaComponent()
{
}

// Component type is the one passed inside QML (registered name)
MetaComponent * MetaComponent::create(const QString &componentType, QObject *parent)
{
    // 1. Try to get the C++ metatype registered from the QML name
    // Right now, we run through all registered QML modules, and get the correct one.
    // If several are found, we break. Not the nicest, but we can do something better later.
    QList<QQmlType *> types = QQmlMetaType::qmlTypes();
    QList<QByteArray> found;
    foreach (const QQmlType *type, types) {
        if (type->elementName() == componentType) {
            found.append(type->typeName());
        }
    }

    if (found.isEmpty()) {
        qWarning() << "No QML component were registered with name" << componentType;
        return 0;
    }

    if (found.count() > 1) {
        qWarning() << "Several QML components were registered with name" << componentType;
        return 0;
    }

    QByteArray cppTypeByteArray = found.first();
    cppTypeByteArray.append("*");
    const char *cppType = QMetaObject::normalizedType(cppTypeByteArray).constData();


    // 2. Try to get the Phonebot MetaData
    int componentTypeIndex = QMetaType::type(cppType);
    if (componentTypeIndex == QMetaType::UnknownType) {
        qWarning() << "Cannot get Phonebot metadata from" << componentType << ":"
                   << "Cannot find Qt metatype for" << cppType
                   << "It might not be registered via qmlRegisterType.";
        return 0;
    }

    const QMetaObject *componentMeta = QMetaType::metaObjectForType(componentTypeIndex);
    int metaClassInfoIndex = componentMeta->indexOfClassInfo("meta");
    if (metaClassInfoIndex == -1) {
        qWarning() << "Cannot get Phonebot metadata from" << componentType << ":"
                   << cppType << "don't have a Phonebot metatype."
                   << "Did you forgot PHONEBOT_METADATA macro ?";
        return 0;
    }

    QMetaClassInfo metaClassInfo = componentMeta->classInfo(metaClassInfoIndex);
    QByteArray metaNameByteArray (metaClassInfo.value());
    metaNameByteArray.append("*");
    const char *metaName = QMetaObject::normalizedType(metaNameByteArray).constData();

    int metaTypeIndex = QMetaType::type(metaName);
    if (metaTypeIndex == QMetaType::UnknownType) {
        qWarning() << "Cannot get Phonebot metadata from" << componentType << ":"
                   << "Cannot find Qt metatype for Phonebot metatype" << metaName
                   << "It might not be registered via qDeclareMetaType.";
        return 0;
    }

    // Check if it inherits from AbstractMetaData
    bool ok = false;
    const QMetaObject *componentMetaMeta = QMetaType::metaObjectForType(metaTypeIndex);
    const QMetaObject *super = componentMetaMeta;
    while (super) {
        if (!ok) {
            ok =  (super->className() == AbstractMetaData::staticMetaObject.className());
        }
        super = super->superClass();
    }

    if (!ok) {
        qWarning() << "Cannot get Phonebot metadata from" << componentType << ":"
                   << "Phonebot metatype class" << metaName << "don't inherit from"
                   << AbstractMetaData::staticMetaObject.className();
        return 0;
    }

    // Construct
    QObject *meta = componentMetaMeta->newInstance();
    if (!meta) {
        qWarning() << "Cannot get Phonebot metadata from" << componentType << ":"
                   << "Phonebot metatype class " << metaName
                   << "don't have an invokable constructor and cannot be created.";
        return 0;
    }
    AbstractMetaData *castedMeta = qobject_cast<AbstractMetaData *>(meta);
    Q_ASSERT(castedMeta);

    QSet<QString> properties;
    for (int i = componentMeta->propertyOffset(); i < componentMeta->propertyCount(); ++i) {
        properties.insert(componentMeta->property(i).name());
    }

    QStringList removedProperties;
    foreach (const QString &property, properties) {
        if (!castedMeta->property(property)) {
            qWarning() << property << "do not have metadata registered";
        }
    }

    foreach (const QString &property, removedProperties) {
        properties.remove(property);
    }

    QStringList propertiesList = properties.toList();
    std::sort(propertiesList.begin(), propertiesList.end());

    MetaComponent *component = new MetaComponent(parent);
    castedMeta->setParent(component);
    component->d_ptr->metaData = castedMeta;
    component->d_ptr->properties = propertiesList;
    return component;
}

QStringList MetaComponent::properties() const
{
    Q_D(const MetaComponent);
    return d->properties;
}

MetaProperty * MetaComponent::property(const QString &name) const
{
    Q_D(const MetaComponent);
    return d->metaData->property(name);
}
