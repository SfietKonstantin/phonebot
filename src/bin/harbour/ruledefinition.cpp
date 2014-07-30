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

#include "ruledefinition.h"
#include <QtCore/QDebug>
#include <QtCore/QGlobalStatic>
#include <QtCore/QSet>
#include <QtCore/QTime>
#include <metatypecache.h>
#include <metaproperty.h>

Q_GLOBAL_STATIC(MetaTypeCache, cache)

RuleDefinition::RuleDefinition(QObject *parent)
    : QObject(parent), m_actions(new RuleDefinitionActionModel(this))
{
}

QString RuleDefinition::name() const
{
    return m_name;
}

void RuleDefinition::setName(const QString &name)
{
    if (m_name != name) {
        m_name = name;
        emit nameChanged();
    }
}

RuleComponentModel * RuleDefinition::trigger() const
{
    return m_components.value(PhoneBotHelper::Trigger);
}

RuleDefinitionActionModel * RuleDefinition::actions() const
{
    return m_actions;
}

RuleComponentModel * RuleDefinition::createTempComponent(int type, int index, const QString &component)
{
    Q_UNUSED(index)
    PhoneBotHelper::ComponentType componentType = static_cast<PhoneBotHelper::ComponentType>(type);
    if (m_tempComponents.contains(componentType)) {
        m_tempComponents.value(componentType)->deleteLater();
        m_tempComponents.remove(componentType);
    }

    RuleComponentModel *componentModel = RuleComponentModel::create(component, this);
    m_tempComponents.insert(static_cast<PhoneBotHelper::ComponentType>(type), componentModel);
    return componentModel;
}

RuleComponentModel * RuleDefinition::createTempClonedComponent(int type, int index)
{
    Q_UNUSED(index)
    PhoneBotHelper::ComponentType componentType = static_cast<PhoneBotHelper::ComponentType>(type);
    if (m_tempComponents.contains(componentType)) {
        m_tempComponents.value(componentType)->deleteLater();
        m_tempComponents.remove(componentType);
    }
    RuleComponentModel *other = m_components.value(componentType);
    if (other) {
        m_tempComponents.insert(componentType, RuleComponentModel::clone(other, this));
    }
    return m_tempComponents.value(componentType);
}

void RuleDefinition::saveComponent(int index)
{
    Q_UNUSED(index)
    saveComponent(PhoneBotHelper::Trigger);
    saveComponent(PhoneBotHelper::Condition);
}

void RuleDefinition::discardComponent(int index)
{
    Q_UNUSED(index)
    qDeleteAll(m_tempComponents);
    m_tempComponents.clear();
}

static QmlObject::Ptr convertComponentModelToObject(RuleComponentModel *componentModel,
                                                    QSet<ImportStatement::Ptr> &imports,
                                                    QList<QmlObject::Ptr> &mappers)
{
    if (!componentModel) {
        return QmlObject::Ptr();
    }

    QString type = componentModel->type();
    QmlObject::Ptr object = QmlObject::create(type);
    imports.insert(cache->import(type));
    QVariantMap properties;

    for (int i = 0; i < componentModel->count(); ++i) {
        QVariant typeVariant = componentModel->data(componentModel->index(i),
                                                    RuleComponentModel::Type);
        MetaProperty *type = typeVariant.value<MetaProperty *>();
        if (type) {
            QVariant value = componentModel->data(componentModel->index(i),
                                                  RuleComponentModel::Value);

            if (type->type() == MetaProperty::Time) {
                // Insert time linker (TODO)
                QmlObject::Ptr timeMapper = QmlObject::create("Mapper");
                QVariantMap timeProperties;
                QTime valueTime = value.toTime();
                timeProperties.insert("hour", valueTime.hour());
                timeProperties.insert("minute", valueTime.minute());
                timeMapper->setProperties(timeProperties);
                QString id = QString("mapper%1").arg(mappers.count());
                timeMapper->setId(id);
                mappers.append(timeMapper);
                QStringList fieldValues;
                fieldValues.append("value");
                properties.insert(type->name(), QVariant::fromValue(Reference(id, fieldValues)));
            } else {
                // Insert key value
                properties.insert(type->name(), value);
            }
        }
    }
    object->setProperties(properties);
    return object;
}

QmlDocumentBase::Ptr RuleDefinition::toDocument() const
{
    WritableQmlDocument::Ptr doc = WritableQmlDocument::create();
    QmlObject::Ptr root = QmlObject::create("Rule");
    QSet<ImportStatement::Ptr> imports;
    imports.insert(ImportStatement::create("org.SfietKonstantin.phonebot", QString(), "1.0", QString()));
    QList<QmlObject::Ptr> mappers;
    QVariantMap properties;

    if (!m_name.trimmed().isEmpty()) {
        properties.insert("name", m_name);
    }

    // Trigger
    QmlObject::Ptr trigger = convertComponentModelToObject(m_components.value(PhoneBotHelper::Trigger),
                                                           imports, mappers);
    if (!trigger.isNull()) {
        properties.insert("trigger", QVariant::fromValue(trigger));
    }

    // Condition
    QmlObject::Ptr condition = convertComponentModelToObject(m_components.value(PhoneBotHelper::Condition),
                                                             imports, mappers);
    if (!condition.isNull()) {
        properties.insert("condition", QVariant::fromValue(trigger));
    }

    // Actions
    QVariantList actions;
    for (int i = 0; i < m_actions->count(); ++i) {
        RuleComponentModel *component = m_actions->data(m_actions->index(i), RuleDefinitionActionModel::Component).value<RuleComponentModel *>();
        QmlObject::Ptr action = convertComponentModelToObject(component, imports, mappers);
        if (!action.isNull()) {
            actions.append(QVariant::fromValue(action));
        }
    }

    if (!actions.isEmpty()) {
        properties.insert("actions", actions);
    }

    // Mappers
    QVariantList mappersVariant;
    foreach (QmlObject::Ptr mapper, mappers) {
        mappersVariant.append(QVariant::fromValue(mapper));
    }

    if (!mappersVariant.isEmpty()) {
        properties.insert("mappers", mappersVariant);
    }

    root->setProperties(properties);
    foreach (ImportStatement::Ptr import, imports) {
        doc->addImport(import);
    }
    doc->setRootObject(root);
    qDebug() << doc->toString();

    return WritableQmlDocument::toBase(doc);
}

void RuleDefinition::saveComponent(PhoneBotHelper::ComponentType type)
{
    if (!m_tempComponents.contains(type)) {
        return;
    }

    RuleComponentModel *component = m_components.value(type);
    RuleComponentModel *newComponent = m_tempComponents.value(type);
    if (component != newComponent) {
        if (component) {
            component->deleteLater();
        }
        m_components.insert(type, newComponent);
        m_tempComponents.remove(type);

        switch (type) {
        case PhoneBotHelper::Trigger:
            emit triggerChanged();
            break;
        case PhoneBotHelper::Condition:
            emit conditionChanged();
            break;
        default:
            break;
        }
    }
}
