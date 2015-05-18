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

#include "rulecomponentmodel.h"
#include "rulecomponentmodel_p.h"
#include <QtCore/QGlobalStatic>
#include <metatypecache.h>

Q_GLOBAL_STATIC(MetaTypeCache, cache)

RuleComponentModelData::~RuleComponentModelData()
{
}

RuleComponentModelPrivate::RuleComponentModelPrivate(RuleComponentModel * q)
    : metaData(0), q_ptr(q)
{
}

RuleComponentModelPrivate::~RuleComponentModelPrivate()
{
}

void RuleComponentModelPrivate::clear()
{
    Q_Q(RuleComponentModel);
    if (m_data.count() > 0) {
        q->beginRemoveRows(QModelIndex(), 0, q->rowCount() - 1);
        qDeleteAll(m_data);
        m_data.clear();
        q->endRemoveRows();
        emit q->countChanged();
    }
}

void RuleComponentModelPrivate::appendData(const QList<RuleComponentModelData *> &data)
{
    Q_Q(RuleComponentModel);
    if (data.count() > 0) {
        q->beginInsertRows(QModelIndex(), q->rowCount(), q->rowCount() + data.count() - 1);
        m_data.append(data);
        emit q->countChanged();
        q->endInsertRows();
    }
}

////// End of private class //////

RuleComponentModel::RuleComponentModel(QObject *parent)
    : QAbstractListModel(parent), d_ptr(new RuleComponentModelPrivate(this))
{
}

RuleComponentModel::RuleComponentModel(RuleComponentModelPrivate &dd, QObject *parent)
    : QAbstractListModel(parent), d_ptr(&dd)
{
}

RuleComponentModel::~RuleComponentModel()
{
    Q_D(RuleComponentModel);
    qDeleteAll(d->m_data);
}

QHash<int, QByteArray> RuleComponentModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles.insert(Type, "type");
    roles.insert(Value, "value");
    return roles;
}

QString RuleComponentModel::type() const
{
    Q_D(const RuleComponentModel);
    return d->type;
}

QString RuleComponentModel::name() const
{
    Q_D(const RuleComponentModel);
    if (!d->metaData) {
        return QString();
    }
    return d->metaData->name();
}

QString RuleComponentModel::description() const
{
    Q_D(const RuleComponentModel);
    if (!d->metaData) {
        return QString();
    }
    return d->metaData->description();
}

QString RuleComponentModel::summary() const
{
    Q_D(const RuleComponentModel);
    if (!d->metaData) {
        return QString();
    }
    QVariantMap properties;
    for (RuleComponentModelData *item : d->m_data) {
        properties.insert(item->type->name(), item->value);
    }
    return d->metaData->summary(properties);
}

int RuleComponentModel::count() const
{
    return rowCount();
}

int RuleComponentModel::rowCount(const QModelIndex &parent) const
{
    Q_D(const RuleComponentModel);
    Q_UNUSED(parent)
    return d->m_data.count();
}

QVariant RuleComponentModel::data(const QModelIndex &index, int role) const
{
    Q_D(const RuleComponentModel);
    if (index.row() < 0 or index.row() >= rowCount()) {
        return QVariant();
    }
    RuleComponentModelData *data = d->m_data.at(index.row());

    switch (role) {
    case Type:
        return QVariant::fromValue(data->type);
        break;
    case Value:
        return data->value;
        break;
    default:
        return QVariant();
        break;
    }
}

RuleComponentModel * RuleComponentModel::create(const QString &type, QObject *parent)
{
    if (!cache->exists(type)) {
        return 0;
    }

    RuleComponentModel *component = new RuleComponentModel(parent);
    component->d_ptr->type = type;
    component->d_ptr->metaData = cache->metaData(type);

    QList<RuleComponentModelData *> items;
    for (const QString &property : cache->properties(type)) {
        RuleComponentModelData *item = new RuleComponentModelData;
        item->type = component->d_ptr->metaData->property(property);
        items.append(item);
    }
    component->d_ptr->appendData(items);
    return component;
}

RuleComponentModel * RuleComponentModel::clone(const RuleComponentModel *other, QObject *parent)
{
    RuleComponentModel *cloned = new RuleComponentModel(parent);
    cloned->d_ptr->type = other->d_ptr->type;
    cloned->d_ptr->metaData = other->d_ptr->metaData;

    QList<RuleComponentModelData *> items;
    for (const RuleComponentModelData *item : other->d_ptr->m_data) {
        RuleComponentModelData *clonedItem = new RuleComponentModelData;
        clonedItem->type = cloned->d_ptr->metaData->property(item->type->name());
        clonedItem->value = item->value;
        items.append(clonedItem);
    }
    cloned->d_ptr->appendData(items);
    return cloned;
}

void RuleComponentModel::setValue(int row, const QVariant &value)
{
    Q_D(RuleComponentModel);
    if (row < 0 || row >= rowCount()) {
        return;
    }

    d->m_data[row]->value = value;
    emit dataChanged(index(row), index(row));
    emit summaryChanged();
}
