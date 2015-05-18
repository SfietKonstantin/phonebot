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

#include "ruledefinitionactionmodel.h"
#include "ruledefinitionactionmodel_p.h"
#include "phonebothelper.h"

RuleDefinitionActionModelData::~RuleDefinitionActionModelData()
{
    component->deleteLater();
}

RuleDefinitionActionModelPrivate::RuleDefinitionActionModelPrivate(RuleDefinitionActionModel * q)
    : q_ptr(q)
{
}

RuleDefinitionActionModelPrivate::~RuleDefinitionActionModelPrivate()
{
}

void RuleDefinitionActionModelPrivate::clear()
{
    Q_Q(RuleDefinitionActionModel);
    if (m_data.count() > 0) {
        q->beginRemoveRows(QModelIndex(), 0, q->rowCount() - 1);
        qDeleteAll(m_data);
        m_data.clear();
        q->endRemoveRows();
        emit q->countChanged();
    }
}

void RuleDefinitionActionModelPrivate::prependData(const QList<RuleDefinitionActionModelData *> &data)
{
    Q_Q(RuleDefinitionActionModel);
    if (data.count() > 0) {
        q->beginInsertRows(QModelIndex(), 0, data.count() - 1);
        QList<RuleDefinitionActionModelData *> oldData = m_data;
        m_data = data;
        m_data.append(oldData);
        emit q->countChanged();
        q->endInsertRows();
    }
}

void RuleDefinitionActionModelPrivate::appendData(const QList<RuleDefinitionActionModelData *> &data)
{
    Q_Q(RuleDefinitionActionModel);
    if (data.count() > 0) {
        q->beginInsertRows(QModelIndex(), q->rowCount(), q->rowCount() + data.count() - 1);
        m_data.append(data);
        emit q->countChanged();
        q->endInsertRows();
    }
}

////// End of private class //////

RuleDefinitionActionModel::RuleDefinitionActionModel(QObject *parent)
    : QAbstractListModel(parent), d_ptr(new RuleDefinitionActionModelPrivate(this))
{
}

RuleDefinitionActionModel::RuleDefinitionActionModel(RuleDefinitionActionModelPrivate &dd, QObject *parent)
    : QAbstractListModel(parent), d_ptr(&dd)
{
}

RuleDefinitionActionModel::~RuleDefinitionActionModel()
{
    Q_D(RuleDefinitionActionModel);
    qDeleteAll(d->m_data);
}

QHash<int, QByteArray> RuleDefinitionActionModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles.insert(Component, "component");
    return roles;
}

int RuleDefinitionActionModel::count() const
{
    return rowCount();
}

int RuleDefinitionActionModel::rowCount(const QModelIndex &parent) const
{
    Q_D(const RuleDefinitionActionModel);
    Q_UNUSED(parent)
    return d->m_data.count();
}

QVariant RuleDefinitionActionModel::data(const QModelIndex &index, int role) const
{
    Q_D(const RuleDefinitionActionModel);
    if (index.row() < 0 or index.row() >= rowCount()) {
        return QVariant();
    }
    RuleDefinitionActionModelData *data = d->m_data.at(index.row());

    switch (role) {
    case Component:
        return QVariant::fromValue(data->component);
        break;
    default:
        return QVariant();
        break;
    }
}

RuleComponentModel * RuleDefinitionActionModel::getComponent(int index) const
{
    Q_D(const RuleDefinitionActionModel);
    if (index < 0 || index > rowCount() + 1) {
        return 0;
    }

    return d->m_data.value(index)->component;
}

RuleComponentModel * RuleDefinitionActionModel::createTempComponent(int type, int index, const QString &component)
{
    Q_D(RuleDefinitionActionModel);
    if (index < 0 || index > rowCount() + 1) {
        return 0;
    }

    if (static_cast<PhoneBotHelper::ComponentType>(type) != PhoneBotHelper::Action) {
        return 0;
    }

    if (d->tempComponents.contains(index)) {
        d->tempComponents.value(index)->deleteLater();
        d->tempComponents.remove(index);
    }

    RuleComponentModel *componentModel = RuleComponentModel::create(component, this);
    d->tempComponents.insert(index, componentModel);
    return componentModel;
}

RuleComponentModel * RuleDefinitionActionModel::createTempClonedComponent(int type, int index)
{
    Q_UNUSED(type)
    if (index < 0 || index > rowCount() + 1) {
        return 0;
    }
    Q_D(RuleDefinitionActionModel);
    if (d->tempComponents.contains(index)) {
        d->tempComponents.value(index)->deleteLater();
        d->tempComponents.remove(index);
    }
    if (index < rowCount()) {
        RuleComponentModel *other = d->m_data.at(index)->component;
        if (other) {
            d->tempComponents.insert(index, RuleComponentModel::clone(other, this));
        }
    }
    return d->tempComponents.value(index);
}

RuleDefinitionActionModel * RuleDefinitionActionModel::clone(const RuleDefinitionActionModel *other,
                                                             QObject *parent)
{
    RuleDefinitionActionModel *cloned = new RuleDefinitionActionModel(parent);
    for (RuleDefinitionActionModelData *item : other->d_ptr->m_data) {
        RuleDefinitionActionModelData *clonedItem = 0;
        if (item) {
            clonedItem = new RuleDefinitionActionModelData;
            clonedItem->component = RuleComponentModel::clone(item->component, cloned);
        }
        cloned->d_ptr->m_data.append(clonedItem);
    }
    return cloned;
}

void RuleDefinitionActionModel::saveComponent(int row)
{
    Q_D(RuleDefinitionActionModel);
    if (row < 0 || row > rowCount() + 1) {
        return;
    }

    if (row < rowCount()) {
        RuleDefinitionActionModelData *item = d->m_data.value(row);
        RuleComponentModel *newComponent = d->tempComponents.value(row);
        if (item->component != newComponent) {
            if (item->component) {
                item->component->deleteLater();
            }
            item->component = newComponent;
        }

        emit dataChanged(index(row), index(row));
    } else {
        QList<RuleDefinitionActionModelData *> data;
        RuleDefinitionActionModelData *item = new RuleDefinitionActionModelData;
        item->component = d->tempComponents.value(row);
        data.append(item);
        d->appendData(data);
    }

    d->tempComponents.remove(row);
}

void RuleDefinitionActionModel::discardComponent(int index)
{
    Q_D(RuleDefinitionActionModel);
    if (index < 0 || index > rowCount() + 1) {
        return;
    }
    d->tempComponents.value(index)->deleteLater();
    d->tempComponents.remove(index);
}

void RuleDefinitionActionModel::remove(int row)
{
    Q_D(RuleDefinitionActionModel);
    if (row < 0 || row >= rowCount()) {
        return;
    }
    beginRemoveRows(QModelIndex(), row, row);
    delete d->m_data.takeAt(row);
    emit countChanged();
    endRemoveRows();
}

