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

#include "metaproperty.h"
#include <QtCore/QVariant>

struct MetaPropertyPrivate
{
    explicit MetaPropertyPrivate();
    QString name;
    MetaProperty::Type type;
    MetaProperty::SubType subType;
    QString description;
    ChoiceModel *choiceModel;
};

MetaPropertyPrivate::MetaPropertyPrivate()
    : choiceModel(0)
{
}

MetaProperty::MetaProperty(QObject *parent)
    : QObject(parent), d_ptr(new MetaPropertyPrivate)
{
}

MetaProperty::~MetaProperty()
{
}

MetaProperty * MetaProperty::create(const QString &name, Type type, const QString &description,
                                    QObject *parent)
{
    MetaProperty *object = new MetaProperty(parent);
    object->d_ptr->name = name;
    object->d_ptr->type = type;
    object->d_ptr->subType = NoSubType;
    object->d_ptr->description = description;
    return object;
}

MetaProperty * MetaProperty::create(const QString &name, Type type, SubType subType,
                                    const QString &description, QObject *parent)
{
    MetaProperty *object = new MetaProperty(parent);
    object->d_ptr->name = name;
    object->d_ptr->type = type;
    object->d_ptr->subType = subType;
    object->d_ptr->description = description;
    return object;
}

MetaProperty * MetaProperty::createString(const QString &name, const QString &description,
                                          QObject *parent)
{
    return create(name, String, description, parent);
}

MetaProperty * MetaProperty::createInt(const QString &name, const QString &description,
                                       QObject *parent)
{
    return create(name, Int, description, parent);
}

MetaProperty * MetaProperty::createDouble(const QString &name, const QString &description,
                                          QObject *parent)
{
    return create(name, Double, description, parent);
}

MetaProperty * MetaProperty::createBool(const QString &name, const QString &description,
                                        QObject *parent)
{
    return create(name, Bool, description, parent);
}

MetaProperty * MetaProperty::createChoice(const QString &name, const QString &description,
                                          ChoiceModel *choice, QObject *parent)
{
    if (!choice) {
        return 0;
    }

    MetaProperty *returned = MetaProperty::create(name, String, ChoiceSubType, description, parent);
    returned->d_ptr->choiceModel = choice;
    returned->d_ptr->choiceModel->setParent(returned);
    return returned;
}

QString MetaProperty::name() const
{
    Q_D(const MetaProperty);
    return d->name;
}

MetaProperty::Type MetaProperty::type() const
{
    Q_D(const MetaProperty);
    return d->type;
}

MetaProperty::SubType MetaProperty::subType() const
{
    Q_D(const MetaProperty);
    return d->subType;
}

QString MetaProperty::description() const
{
    Q_D(const MetaProperty);
    return d->description;
}

ChoiceModel * MetaProperty::choiceModel() const
{
    Q_D(const MetaProperty);
    return d->choiceModel;
}

bool MetaProperty::isCompatible(QMetaType::Type other) const
{
    Q_D(const MetaProperty);
    return QVariant((QVariant::Type) d->type).canConvert(other);
}
