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

#include "choicemodel.h"
#include "choicemodel_p.h"

ChoiceModelData::~ChoiceModelData()
{
}

ChoiceModelPrivate::ChoiceModelPrivate(ChoiceModel * q)
    : q_ptr(q)
{
}

ChoiceModelPrivate::~ChoiceModelPrivate()
{
}

void ChoiceModelPrivate::clear()
{
    Q_Q(ChoiceModel);
    if (m_data.count() > 0) {
        q->beginRemoveRows(QModelIndex(), 0, q->rowCount() - 1);
        qDeleteAll(m_data);
        m_data.clear();
        m_map.clear();
        q->endRemoveRows();
        emit q->countChanged();
    }
}

void ChoiceModelPrivate::appendData(ChoiceModelData *data)
{
    Q_Q(ChoiceModel);
    q->beginInsertRows(QModelIndex(), q->rowCount(), q->rowCount());
    m_data.append(data);
    m_map.insert(data->value, data->name);
    emit q->countChanged();
    q->endInsertRows();
}

////// End of private class //////

ChoiceModel::ChoiceModel(QObject *parent)
    : QAbstractListModel(parent), d_ptr(new ChoiceModelPrivate(this))
{
}

ChoiceModel::ChoiceModel(ChoiceModelPrivate &dd, QObject *parent)
    : QAbstractListModel(parent), d_ptr(&dd)
{
}

ChoiceModel::~ChoiceModel()
{
    Q_D(ChoiceModel);
    qDeleteAll(d->m_data);
}

QHash<int, QByteArray> ChoiceModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles.insert(Name, "name");
    roles.insert(Value, "value");
    return roles;
}

int ChoiceModel::count() const
{
    return rowCount();
}

int ChoiceModel::rowCount(const QModelIndex &parent) const
{
    Q_D(const ChoiceModel);
    Q_UNUSED(parent)
    return d->m_data.count();
}

QVariant ChoiceModel::data(const QModelIndex &index, int role) const
{
    Q_D(const ChoiceModel);
    if (index.row() < 0 or index.row() >= rowCount()) {
        return QVariant();
    }
    ChoiceModelData *data = d->m_data.at(index.row());
    switch (role) {
    case Name:
        return data->name;
        break;
    case Value:
        return data->value;
        break;
    default:
        return QVariant();
        break;
    }
}

QString ChoiceModel::nameFromValue(const QString &value) const
{
    Q_D(const ChoiceModel);
    return d->m_map.value(value);
}

void ChoiceModel::addEntry(const QString &name, const QString &value)
{
    Q_D(ChoiceModel);
    ChoiceModelData *data = new ChoiceModelData;
    data->name = name;
    data->value = value;
    d->appendData(data);
}

void ChoiceModel::clear()
{
    Q_D(ChoiceModel);
    d->clear();
}
