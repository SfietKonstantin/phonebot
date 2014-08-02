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

#include "weekdaycondition.h"
#include <condition_p.h>
#include <QtCore/QDate>

class WeekDayConditionPrivate: public ConditionPrivate
{
public:
    WeekDayConditionPrivate(WeekDayCondition *q);
    bool onMonday;
    bool onTuesday;
    bool onWednesday;
    bool onThursday;
    bool onFriday;
    bool onSaturday;
    bool onSunday;
};

WeekDayConditionPrivate::WeekDayConditionPrivate(WeekDayCondition *q)
    : ConditionPrivate(q), onMonday(false), onTuesday(false), onWednesday(false), onThursday(false)
    , onFriday(false), onSaturday(false), onSunday(false)
{
}

WeekDayCondition::WeekDayCondition(QObject *parent) :
    Condition(*(new WeekDayConditionPrivate(this)), parent)
{
}

bool WeekDayCondition::isOnMonday() const
{
    Q_D(const WeekDayCondition);
    return d->onMonday;
}

void WeekDayCondition::setOnMonday(bool onMonday)
{
    Q_D(WeekDayCondition);
    if (d->onMonday != onMonday) {
        d->onMonday = onMonday;
        emit onMondayChanged();
    }
}

bool WeekDayCondition::isOnTuesday() const
{
    Q_D(const WeekDayCondition);
    return d->onTuesday;
}

void WeekDayCondition::setOnTuesday(bool onTuesday)
{
    Q_D(WeekDayCondition);
    if (d->onTuesday != onTuesday) {
        d->onTuesday = onTuesday;
        emit onTuesdayChanged();
    }
}

bool WeekDayCondition::isOnWednesday() const
{
    Q_D(const WeekDayCondition);
    return d->onWednesday;
}

void WeekDayCondition::setOnWednesday(bool onWednesday)
{
    Q_D(WeekDayCondition);
    if (d->onWednesday != onWednesday) {
        d->onWednesday = onWednesday;
        emit onWednesdayChanged();
    }
}

bool WeekDayCondition::isOnThursday() const
{
    Q_D(const WeekDayCondition);
    return d->onThursday;
}

void WeekDayCondition::setOnThursday(bool onThursday)
{
    Q_D(WeekDayCondition);
    if (d->onThursday != onThursday) {
        d->onThursday = onThursday;
        emit onThursdayChanged();
    }
}

bool WeekDayCondition::isOnFriday() const
{
    Q_D(const WeekDayCondition);
    return d->onFriday;
}

void WeekDayCondition::setOnFriday(bool onFriday)
{
    Q_D(WeekDayCondition);
    if (d->onFriday != onFriday) {
        d->onFriday = onFriday;
        emit onFridayChanged();
    }
}

bool WeekDayCondition::isOnSaturday() const
{
    Q_D(const WeekDayCondition);
    return d->onSaturday;
}

void WeekDayCondition::setOnSaturday(bool onSaturday)
{
    Q_D(WeekDayCondition);
    if (d->onSaturday != onSaturday) {
        d->onSaturday = onSaturday;
        emit onSaturdayChanged();
    }
}

bool WeekDayCondition::isOnSunday() const
{
    Q_D(const WeekDayCondition);
    return d->onSunday;
}

void WeekDayCondition::setOnSunday(bool onSunday)
{
    Q_D(WeekDayCondition);
    if (d->onSunday != onSunday) {
        d->onSunday = onSunday;
        emit onSundayChanged();
    }
}

bool WeekDayCondition::isValid(Rule *rule)
{
    Q_D(WeekDayCondition);
    Q_UNUSED(rule);
    int day = QDate::currentDate().dayOfWeek();
    if (day == 1 && d->onMonday) {
        return true;
    }
    if (day == 2 && d->onTuesday) {
        return true;
    }
    if (day == 3 && d->onWednesday) {
        return true;
    }
    if (day == 4 && d->onThursday) {
        return true;
    }
    if (day == 5 && d->onFriday) {
        return true;
    }
    if (day == 6 && d->onSaturday) {
        return true;
    }
    if (day == 7 && d->onSunday) {
        return true;
    }

    return false;
}

WeekDayConditionMeta::WeekDayConditionMeta(QObject *parent)
    : AbstractMetaData(parent)
{
}

QString WeekDayConditionMeta::name() const
{
    return tr("Day of week");
}

QString WeekDayConditionMeta::description() const
{
    return tr("This condition will check if today matches a given set of day of week.");
}

MetaProperty * WeekDayConditionMeta::getProperty(const QString &property, QObject *parent) const
{
    if (property == "onMonday") {
        return MetaProperty::createBool(property, tr("Monday"), parent);
    }
    if (property == "onTuesday") {
        return MetaProperty::createBool(property, tr("Tuesday"), parent);
    }
    if (property == "onWednesday") {
        return MetaProperty::createBool(property, tr("Wednesday"), parent);
    }
    if (property == "onThursday") {
        return MetaProperty::createBool(property, tr("Thursday"), parent);
    }
    if (property == "onFriday") {
        return MetaProperty::createBool(property, tr("Friday"), parent);
    }
    if (property == "onSaturday") {
        return MetaProperty::createBool(property, tr("Saturday"), parent);
    }
    if (property == "onSunday") {
        return MetaProperty::createBool(property, tr("Sunday"), parent);
    }
    return 0;
}
