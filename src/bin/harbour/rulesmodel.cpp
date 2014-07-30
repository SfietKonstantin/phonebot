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
#include <QtCore/QDir>
#include <QtCore/QStandardPaths>
#include <qmldocument.h>
#include "ruledefinition.h"

static const char *RULE_FILE = "rule.qml";

struct RulesModelData
{
    RuleDefinition *definition;
};

RulesModel::RulesModel(QObject *parent) :
    QAbstractListModel(parent)
{
}

RulesModel::~RulesModel()
{
    qDeleteAll(m_data);
}

QHash<int, QByteArray> RulesModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles.insert(NameRole, "name");
    roles.insert(DefinitionRole, "definition");
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
    case DefinitionRole:
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

void RulesModel::reload()
{
    if (!m_data.isEmpty()) {
        beginRemoveRows(QModelIndex(), 0, rowCount() - 1);
        qDeleteAll(m_data);
        m_data.clear();
        endRemoveRows();
    }

    // We check every folder inside .config/<org>/<app>/
    // and see if there is a "rule.qml" inside
    QString configRoot = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    configRoot.append(QString("/%1/%2/").arg(QCoreApplication::instance()->organizationName(),
                                             QCoreApplication::instance()->applicationName()));
    qDebug() << "Using" << configRoot << "to search rules";
    QDir dir (configRoot);
    foreach (const QString &subdirPath, dir.entryList(QDir::Dirs)) {
        QDir subdir (dir);
        if (!subdir.cd(subdirPath)) {
            continue;
        }
        if (subdir.exists(RULE_FILE)) {
            QString rule = subdir.absoluteFilePath(RULE_FILE);
            qDebug() << "Rule found:" << rule;

            // Parse rule
            QmlDocument::Ptr doc = QmlDocument::create(rule);
            if (doc->error() == QmlDocument::NoError) {
                QmlObject::Ptr root = doc->rootObject();
                QString name = root->property("name").toString();
                QString triggerType;
                if (root->hasProperty("trigger")) {
                    QmlObject::Ptr trigger = root->property("trigger").value<QmlObject::Ptr>();
                    if (!trigger.isNull()) {
                        triggerType = trigger->type();
                        // Parse other properties (values)
                    }
                }

                QString conditionType;
                if (root->hasProperty("condition")) {
                    QmlObject::Ptr condition = root->property("condition").value<QmlObject::Ptr>();
                    if (!condition.isNull()) {
                        conditionType = condition->type();
                        // Parse other properties (values)
                    }
                }

                RuleDefinition *ruleDefinition = new RuleDefinition(this);
                ruleDefinition->setName(name);
//                MetaComponent *triggerMeta = MetaComponent::create(triggerType, this);
//                if (triggerMeta) {
//                    ruleDefinition->setTrigger(triggerMeta);
//                }

//                MetaComponent *conditionMeta = MetaComponent::create(conditionType, this);
//                if (conditionMeta) {
//                    ruleDefinition->setTrigger(conditionMeta);
//                }

                RulesModelData *data = new RulesModelData;
                data->definition = ruleDefinition;
                m_data.append(data);
            }
        }
    }

    beginInsertRows(QModelIndex(), 0, m_data.count() - 1);
    emit countChanged();
    endInsertRows();
}

bool RulesModel::pushRule(RuleDefinition *rule)
{
    rule->toDocument();

    beginInsertRows(QModelIndex(), m_data.count() - 1, m_data.count());
    RulesModelData *data = new RulesModelData;
    data->definition = rule;
    m_data.append(data);
    emit countChanged();
    endInsertRows();

    // TODO: check and save rule
    return true;
}

void RulesModel::discardRule(RuleDefinition *rule) const
{
    rule->deleteLater();
}
