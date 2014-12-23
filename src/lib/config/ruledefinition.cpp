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

class RuleDefinitionPrivate
{
public:
    explicit RuleDefinitionPrivate(RuleDefinition *q);
    void saveComponent(PhoneBotHelper::ComponentType type);
    QString name;

    QMap<PhoneBotHelper::ComponentType, RuleComponentModel *> components;
    QMap<PhoneBotHelper::ComponentType, RuleComponentModel *> tempComponents;
    RuleDefinitionActionModel *actions;
protected:
    RuleDefinition * const q_ptr;
private:
    Q_DECLARE_PUBLIC(RuleDefinition)
};

RuleDefinitionPrivate::RuleDefinitionPrivate(RuleDefinition *q)
    : actions (0), q_ptr(q)
{
}

void RuleDefinitionPrivate::saveComponent(PhoneBotHelper::ComponentType type)
{
    Q_Q(RuleDefinition);
    if (!tempComponents.contains(type)) {
        return;
    }

    RuleComponentModel *component = components.value(type);
    RuleComponentModel *newComponent = tempComponents.value(type);
    if (component != newComponent) {
        if (component) {
            component->deleteLater();
        }
        components.insert(type, newComponent);

        switch (type) {
        case PhoneBotHelper::Trigger:
            emit q->triggerChanged();
            break;
        case PhoneBotHelper::Condition:
            emit q->conditionChanged();
            break;
        default:
            break;
        }
    }
    tempComponents.remove(type);
}


RuleDefinition::RuleDefinition(QObject *parent)
    : QObject(parent), d_ptr(new RuleDefinitionPrivate(this))
{
    Q_D(RuleDefinition);
    d->actions = new RuleDefinitionActionModel(this);
}

RuleDefinition::~RuleDefinition()
{
}

QString RuleDefinition::name() const
{
    Q_D(const RuleDefinition);
    return d->name;
}

void RuleDefinition::setName(const QString &name)
{
    Q_D(RuleDefinition);
    if (d->name != name) {
        d->name = name;
        emit nameChanged();
    }
}

RuleComponentModel * RuleDefinition::trigger() const
{
    Q_D(const RuleDefinition);
    return d->components.value(PhoneBotHelper::Trigger);
}

RuleComponentModel * RuleDefinition::condition() const
{
    Q_D(const RuleDefinition);
    return d->components.value(PhoneBotHelper::Condition);
}

RuleDefinitionActionModel * RuleDefinition::actions() const
{
    Q_D(const RuleDefinition);
    return d->actions;
}

RuleComponentModel * RuleDefinition::createTempComponent(int type, int index, const QString &component)
{
    Q_D(RuleDefinition);
    Q_UNUSED(index)
    PhoneBotHelper::ComponentType componentType = static_cast<PhoneBotHelper::ComponentType>(type);
    if (d->tempComponents.contains(componentType)) {
        d->tempComponents.value(componentType)->deleteLater();
        d->tempComponents.remove(componentType);
    }

    RuleComponentModel *componentModel = RuleComponentModel::create(component, this);
    d->tempComponents.insert(static_cast<PhoneBotHelper::ComponentType>(type), componentModel);
    return componentModel;
}

RuleComponentModel * RuleDefinition::createTempClonedComponent(int type, int index)
{
    Q_D(RuleDefinition);
    Q_UNUSED(index)
    PhoneBotHelper::ComponentType componentType = static_cast<PhoneBotHelper::ComponentType>(type);
    if (d->tempComponents.contains(componentType)) {
        d->tempComponents.value(componentType)->deleteLater();
        d->tempComponents.remove(componentType);
    }
    RuleComponentModel *other = d->components.value(componentType);
    if (other) {
        d->tempComponents.insert(componentType, RuleComponentModel::clone(other, this));
    }
    return d->tempComponents.value(componentType);
}

void RuleDefinition::saveComponent(int index)
{
    Q_D(RuleDefinition);
    Q_UNUSED(index)
    d->saveComponent(PhoneBotHelper::Trigger);
    d->saveComponent(PhoneBotHelper::Condition);
}

void RuleDefinition::discardComponent(int index)
{
    Q_D(RuleDefinition);
    Q_UNUSED(index)
    qDeleteAll(d->tempComponents);
    d->tempComponents.clear();
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

            // Workaround for null QVariant
            if (type->type() == MetaProperty::Bool) {
                if (value.isNull()) {
                    value = QVariant(false);
                }
            }

            if (type->type() == MetaProperty::Time) {
                // Insert time linker (TODO)
                QmlObject::Ptr timeMapper = QmlObject::create("TimeMapper");
                QVariantMap timeProperties;
                QTime valueTime = value.toTime();
                if (valueTime.isValid()) {
                    timeProperties.insert("hour", valueTime.hour());
                    timeProperties.insert("minute", valueTime.minute());
                    timeMapper->setProperties(timeProperties);
                    QString id = QString("mapper%1").arg(mappers.count());
                    timeMapper->setId(id);
                    mappers.append(timeMapper);
                    QStringList fieldValues;
                    fieldValues.append("value");
                    properties.insert(type->name(), QVariant::fromValue(Reference(id, fieldValues)));
                }
            } else {
                // Insert key value
                properties.insert(type->name(), value);
            }
        }
    }
    object->setProperties(properties);
    return object;
}

RuleDefinition * RuleDefinition::clone(const RuleDefinition *other, QObject *parent)
{
    RuleDefinition *cloned = new RuleDefinition(parent);
    cloned->d_ptr->name = other->d_ptr->name;
    if (other->d_ptr->components.contains(PhoneBotHelper::Trigger)) {
        RuleComponentModel *trigger = other->d_ptr->components.value(PhoneBotHelper::Trigger);
        if (trigger) {
            cloned->d_ptr->components.insert(PhoneBotHelper::Trigger, trigger->clone(trigger, cloned));
        } else {
            cloned->d_ptr->components.insert(PhoneBotHelper::Trigger, 0);
        }
    }

    if (other->d_ptr->components.contains(PhoneBotHelper::Condition)) {
        RuleComponentModel *condition = other->d_ptr->components.value(PhoneBotHelper::Condition);
        if (condition) {
            cloned->d_ptr->components.insert(PhoneBotHelper::Condition, condition->clone(condition, cloned));
        } else {
            cloned->d_ptr->components.insert(PhoneBotHelper::Condition, 0);
        }
    }

    cloned->d_ptr->actions = RuleDefinitionActionModel::clone(other->d_ptr->actions, cloned);
    return cloned;
}

QmlDocumentBase::Ptr RuleDefinition::toDocument() const
{
    Q_D(const RuleDefinition);
    WritableQmlDocument::Ptr doc = WritableQmlDocument::create();
    QmlObject::Ptr root = QmlObject::create("Rule");
    QSet<ImportStatement::Ptr> imports;
    imports.insert(ImportStatement::create("org.SfietKonstantin.phonebot", QString(), "1.0", QString()));
    QList<QmlObject::Ptr> mappers;
    QVariantMap properties;

    if (!d->name.trimmed().isEmpty()) {
        properties.insert("name", d->name);
    }

    // Trigger
    QmlObject::Ptr trigger = convertComponentModelToObject(d->components.value(PhoneBotHelper::Trigger),
                                                           imports, mappers);
    if (!trigger.isNull()) {
        properties.insert("trigger", QVariant::fromValue(trigger));
    }

    // Condition
    QmlObject::Ptr condition = convertComponentModelToObject(d->components.value(PhoneBotHelper::Condition),
                                                             imports, mappers);
    if (!condition.isNull()) {
        properties.insert("condition", QVariant::fromValue(condition));
    }

    // Actions
    QVariantList actions;
    for (int i = 0; i < d->actions->count(); ++i) {
        RuleComponentModel *component = d->actions->data(d->actions->index(i), RuleDefinitionActionModel::Component).value<RuleComponentModel *>();
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
    return WritableQmlDocument::toBase(doc);
}
