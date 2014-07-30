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

#ifndef METAPROPERTY_H
#define METAPROPERTY_H

#include <QtCore/QObject>
#include <QtCore/QSharedPointer>
#include <QtCore/QMetaType>
#include <basetype.h>
#include "choicemodel.h"

class MetaPropertyPrivate;
class MetaProperty: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString description READ description CONSTANT)
    Q_PROPERTY(MetaProperty::Type type READ type CONSTANT)
    Q_PROPERTY(MetaProperty::SubType subType READ subType CONSTANT)
    Q_PROPERTY(ChoiceModel * choiceModel READ choiceModel CONSTANT)
    Q_ENUMS(Type)
    Q_ENUMS(SubType)
public:
    enum Type {
        String = StringType,
        Int = IntType,
        Double = DoubleType,
        Bool = BoolType,
        Time = TimeType
    };
    enum SubType {
        NoSubType,
        ChoiceSubType
    };

    explicit MetaProperty(QObject *parent = 0);
    virtual ~MetaProperty();
    static MetaProperty * create(const QString &name, Type type, const QString &description,
                                 QObject *parent = 0);
    static MetaProperty * create(const QString &name, Type type, SubType subType,
                                 const QString &description, QObject *parent = 0);
    static MetaProperty * createString(const QString &name, const QString &description,
                                       QObject *parent = 0);
    static MetaProperty * createInt(const QString &name, const QString &description,
                                    QObject *parent = 0);
    static MetaProperty * createDouble(const QString &name, const QString &description,
                                       QObject *parent = 0);
    static MetaProperty * createBool(const QString &name, const QString &description,
                                     QObject *parent = 0);
    static MetaProperty * createChoice(const QString &name, const QString &description,
                                       ChoiceModel *choice, QObject *parent = 0);
    QString name() const;
    Type type() const;
    SubType subType() const;
    QString description() const;
    ChoiceModel * choiceModel() const;
    bool isCompatible(QMetaType::Type other) const;
protected:
    QScopedPointer<MetaPropertyPrivate> d_ptr;
private:
    Q_DECLARE_PRIVATE(MetaProperty)
};

#endif // METAPROPERTY_H
