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

#include "rulecomponentsmodel.h"
#include "rulecomponentsmodel_p.h"
#include <abstractmetadata.h>
#include <metatypecache.h>
#include <QtCore/QDebug>

Q_GLOBAL_STATIC(MetaTypeCache, cache)

RuleComponentsModelData::~RuleComponentsModelData()
{
}

RuleComponentsModelPrivate::RuleComponentsModelPrivate(RuleComponentsModel * q)
    : type(PhoneBotHelper::Invalid), q_ptr(q)
{
}

RuleComponentsModelPrivate::~RuleComponentsModelPrivate()
{
}

void RuleComponentsModelPrivate::clear()
{
    Q_Q(RuleComponentsModel);
    if (m_data.count() > 0) {
        q->beginRemoveRows(QModelIndex(), 0, q->rowCount() - 1);
        qDeleteAll(m_data);
        m_data.clear();
        q->endRemoveRows();
        emit q->countChanged();
    }
}

void RuleComponentsModelPrivate::prependData(const QList<RuleComponentsModelData *> &data)
{
    Q_Q(RuleComponentsModel);
    if (data.count() > 0) {
        q->beginInsertRows(QModelIndex(), 0, data.count() - 1);
        QList<RuleComponentsModelData *> oldData = m_data;
        m_data = data;
        m_data.append(oldData);
        emit q->countChanged();
        q->endInsertRows();
    }
}

void RuleComponentsModelPrivate::appendData(const QList<RuleComponentsModelData *> &data)
{
    Q_Q(RuleComponentsModel);
    if (data.count() > 0) {
        q->beginInsertRows(QModelIndex(), q->rowCount(), q->rowCount() + data.count() - 1);
        m_data.append(data);
        emit q->countChanged();
        q->endInsertRows();
    }
}

////// End of private class //////

RuleComponentsModel::RuleComponentsModel(QObject *parent)
    : QAbstractListModel(parent), d_ptr(new RuleComponentsModelPrivate(this))
{
}

RuleComponentsModel::RuleComponentsModel(RuleComponentsModelPrivate &dd, QObject *parent)
    : QAbstractListModel(parent), d_ptr(&dd)
{
}

RuleComponentsModel::~RuleComponentsModel()
{
    Q_D(RuleComponentsModel);
    qDeleteAll(d->m_data);
}

QHash<int, QByteArray> RuleComponentsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles.insert(Type, "type");
    roles.insert(Name, "name");
    roles.insert(Description, "description");
    return roles;
}

PhoneBotHelper::ComponentType RuleComponentsModel::type() const
{
    Q_D(const RuleComponentsModel);
    return d->type;
}

void RuleComponentsModel::setType(PhoneBotHelper::ComponentType type)
{
    Q_D(RuleComponentsModel);
    if (d->type != type) {
        d->type = type;
        emit typeChanged();

//        if (!cache.exists()) {
//            qWarning() << "Components cache not initialized";
//            return;
//        }
        QStringList components = cache->components((MetaTypeCache::Type) type);

        QList<RuleComponentsModelData *> items;
        foreach (const QString &component, components) {
            RuleComponentsModelData *item = new RuleComponentsModelData;
            item->type = component;
            AbstractMetaData *meta = cache->metaData(component);
            item->name = meta->name();
            item->description = meta->description();
            items.append(item);
        }
        d->appendData(items);
    }
}

int RuleComponentsModel::count() const
{
    return rowCount();
}

int RuleComponentsModel::rowCount(const QModelIndex &parent) const
{
    Q_D(const RuleComponentsModel);
    Q_UNUSED(parent)
    return d->m_data.count();
}

QVariant RuleComponentsModel::data(const QModelIndex &index, int role) const
{
    Q_D(const RuleComponentsModel);
    if (index.row() < 0 or index.row() >= rowCount()) {
        return QVariant();
    }
    RuleComponentsModelData *data = d->m_data.at(index.row());

    switch (role) {
    case Type:
        return data->type;
        break;
    case Name:
        return data->name;
        break;
    case Description:
        return data->description;
        break;
    default:
        return QVariant();
        break;
    }
}

