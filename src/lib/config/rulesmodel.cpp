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

#include "rulesmodel.h"
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <qmldocument.h>
#include <metaproperty.h>
#include "ruledefinition.h"

static const char *SERVICE = "org.SfietKonstantin.phonebot";
static const char *PATH = "/";

struct RulesModelData
{
    QString path;
    RuleDefinition *definition;
};

RulesModel::RulesModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_proxy(new OrgSfietKonstantinPhonebotInterface(SERVICE, PATH, QDBusConnection::sessionBus(), this))
{
    reload();
}

RulesModel::~RulesModel()
{
    qDeleteAll(m_data);
}

QHash<int, QByteArray> RulesModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles.insert(NameRole, "name");
    roles.insert(RuleRole, "rule");
    return roles;
}

int RulesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_data.count();
}

QVariant RulesModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if (row < 0 || row >= rowCount()) {
        return QVariant();
    }

    const RulesModelData *data = m_data.at(row);
    switch (role) {
    case NameRole:
        return data->definition->name();
        break;
    case RuleRole:
        return QVariant::fromValue(data->definition);
        break;
    default:
        return QVariant();
        break;
    }
}

int RulesModel::count() const
{
    return rowCount();
}

RuleDefinition * RulesModel::createRule()
{
    return new RuleDefinition(this);
}

RuleDefinition * RulesModel::createClonedRule(RuleDefinition *other)
{
    if (!other) {
        return 0;
    }

    return RuleDefinition::clone(other, this);
}

static void populateRuleComponentModel(RuleComponentModel *component, QmlObject::Ptr object,
                                       const QMap<QString, QmlObject::Ptr> &mappers)
{
    if (!component) {
        return;
    }

    for (int i = 0; i < component->count(); ++i) {
        QVariant type = component->data(component->index(i), RuleComponentModel::Type);
        MetaProperty *meta = type.value<MetaProperty *>();
        if (meta) {
            QString key = meta->name();
            if (object->hasProperty(key)) {
                // TODO: migrate to MetaProperty
                QVariant value = object->property(key);
                if (value.canConvert<Reference>()) {
                    Reference reference = value.value<Reference>();
                    if (mappers.contains(reference.identifier())) {
                        // Check mapper
                        QmlObject::Ptr mapper = mappers.value(reference.identifier());
                        if (mapper->type() == "TimeMapper") {
                            int hour = mapper->property("hour").toInt();
                            int minute = mapper->property("minute").toInt();
                            QTime time = QTime(hour, minute);
                            component->setValue(i, QVariant(time));
                        }
                    }
                } else {
                    component->setValue(i, value);
                }
            }
        }
    }
}

void RulesModel::reload()
{
    if (!m_data.isEmpty()) {
        beginRemoveRows(QModelIndex(), 0, rowCount() - 1);
        qDeleteAll(m_data);
        m_data.clear();
        endRemoveRows();
    }

    QStringList rules = m_proxy->Rules();
    foreach (const QString &rule, rules) {
        // Parse rule
        QmlDocument::Ptr doc = QmlDocument::create(rule);

        if (doc->error() == QmlDocument::NoError) {
            RuleDefinition *ruleDefinition = new RuleDefinition(this);
            QmlObject::Ptr root = doc->rootObject();

            // 1. Get a list of mappers
            QMap<QString, QmlObject::Ptr> mappers;
            if (root->hasProperty("mappers")) {
                QVariantList mappersVariant = root->property("mappers").toList();
                foreach (const QVariant &mapperVariant, mappersVariant) {
                    if (mapperVariant.canConvert<QmlObject::Ptr>()) {
                        QmlObject::Ptr mapper = mapperVariant.value<QmlObject::Ptr>();
                        mappers.insert(mapper->id(), mapper);
                    }
                }
            }


            // 2. Name
            QString name = root->property("name").toString();

            // 3. Trigger and condition
            QString triggerType;
            if (root->hasProperty("trigger")) {
                QmlObject::Ptr trigger = root->property("trigger").value<QmlObject::Ptr>();
                if (!trigger.isNull()) {
                    triggerType = trigger->type();
                    RuleComponentModel *triggerComponent = ruleDefinition->createTempComponent(PhoneBotHelper::Trigger, -1, triggerType);
                    populateRuleComponentModel(triggerComponent, trigger, mappers);
                }
            }

            QString conditionType;
            if (root->hasProperty("condition")) {
                QmlObject::Ptr condition = root->property("condition").value<QmlObject::Ptr>();
                if (!condition.isNull()) {
                    conditionType = condition->type();
                    RuleComponentModel *conditionComponent = ruleDefinition->createTempComponent(PhoneBotHelper::Condition, -1, conditionType);
                    populateRuleComponentModel(conditionComponent, condition, mappers);
                }
            }

            ruleDefinition->saveComponent(-1);
            ruleDefinition->setName(name);

            // 4. Actions
            if (root->hasProperty("actions")) {
                QVariantList actionsVariant = root->property("actions").toList();
                RuleDefinitionActionModel *actions = ruleDefinition->actions();
                foreach (const QVariant &actionVariant, actionsVariant) {
                    if (actionVariant.canConvert<QmlObject::Ptr>()) {
                        int index = actions->count();
                        QmlObject::Ptr action = actionVariant.value<QmlObject::Ptr>();
                        RuleComponentModel *actionModel = actions->createTempComponent(PhoneBotHelper::Action,
                                                                                       index, action->type());
                        populateRuleComponentModel(actionModel, action, mappers);
                        actions->saveComponent(index);
                    }
                }
            }

            // 5. Save everything
            RulesModelData *data = new RulesModelData;
            data->path = rule;
            data->definition = ruleDefinition;
            m_data.append(data);
        }
    }

    beginInsertRows(QModelIndex(), 0, m_data.count() - 1);
    emit countChanged();
    endInsertRows();
}

bool RulesModel::pushRule(int index, RuleDefinition *rule)
{
    if (index < 0 || index > rowCount() + 1) {
        return false;
    }

    QString qml = rule->toDocument()->toString();
    if (index < rowCount()) {
        RulesModelData *item = m_data.at(index);
        m_proxy->EditRule(item->path, qml);
    } else {
        m_proxy->AddRule(qml);
    }

    reload();
    return true;
}

void RulesModel::discardRule(RuleDefinition *rule) const
{
    rule->deleteLater();
}

void RulesModel::removeRule(int index)
{
    if (index < 0 || index >= rowCount()) {
        return;
    }

    RulesModelData *item = m_data.at(index);
    m_proxy->RemoveRule(item->path);
    reload();
}
