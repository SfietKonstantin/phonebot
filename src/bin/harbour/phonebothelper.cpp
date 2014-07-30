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

#include "phonebothelper.h"
#include <QtCore/QVariant>
#include <QtCore/QTime>
#include "metaproperty.h"

PhoneBotHelper::PhoneBotHelper(QObject *parent) :
    QObject(parent)
{
}

QString PhoneBotHelper::componentName(ComponentType type)
{
    switch (type) {
    case Trigger:
        return tr("Trigger");
        break;
    case Condition:
        return tr("Condition");
        break;
    case Action:
        return tr("Action");
        break;
    default:
        return QString();
        break;
    }
}

QString PhoneBotHelper::selectComponent(ComponentType type)
{
    switch (type) {
    case Trigger:
        return tr("Select trigger");
        break;
    case Condition:
        return tr("Select condition");
        break;
    case Action:
        return tr("Select action");
        break;
    default:
        return QString();
        break;
    }
}

QString PhoneBotHelper::convert(int type, const QVariant &value)
{
    if (value.isNull()) {
            return QString();
    }

    MetaProperty::Type castedType = static_cast<MetaProperty::Type>(type);
    switch (castedType) {
    case MetaProperty::Time:
        return value.toTime().toString(tr("hh:mm"));
        break;
    default:
        return value.toString();
        break;
    }
}
