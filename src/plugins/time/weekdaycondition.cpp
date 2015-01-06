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
#include <QtCore/QSet>
#include <QtCore/QStringList>

static const char *ONMONDAY_KEY = "onMonday";
static const char *ONTUESDAY_KEY = "onTuesday";
static const char *ONWEDNESDAY_KEY = "onWednesday";
static const char *ONTHURSDAY_KEY = "onThursday";
static const char *ONFRIDAY_KEY = "onFriday";
static const char *ONSATURDAY_KEY = "onSaturday";
static const char *ONSUNDAY_KEY = "onSunday";

static const char *keyFromDay(int day)
{
    switch(day) {
    case 1:
        return ONMONDAY_KEY;
        break;
    case 2:
        return ONTUESDAY_KEY;
        break;
    case 3:
        return ONWEDNESDAY_KEY;
        break;
    case 4:
        return ONTHURSDAY_KEY;
        break;
    case 5:
        return ONFRIDAY_KEY;
        break;
    case 6:
        return ONSATURDAY_KEY;
        break;
    case 7:
        return ONSUNDAY_KEY;
        break;
    default:
        return 0;
        break;
    }
}

class WeekDayConditionPrivate: public ConditionPrivate
{
public:
    WeekDayConditionPrivate(WeekDayCondition *q);
    bool setDay(int day, bool checked);
    QSet<int> checkedDays;
};

WeekDayConditionPrivate::WeekDayConditionPrivate(WeekDayCondition *q)
    : ConditionPrivate(q)
{
}

bool WeekDayConditionPrivate::setDay(int day, bool checked)
{
    if (day < 1 || day > 7) {
        return false;
    }

    bool current = checkedDays.contains(day);
    if (current != checked) {
        if (checked) {
            checkedDays.insert(day);
        } else {
            checkedDays.remove(day);
        }
        return true;
    }
    return false;
}

WeekDayCondition::WeekDayCondition(QObject *parent) :
    Condition(*(new WeekDayConditionPrivate(this)), parent)
{
}

bool WeekDayCondition::isOnMonday() const
{
    Q_D(const WeekDayCondition);
    return d->checkedDays.contains(1);
}

void WeekDayCondition::setOnMonday(bool onMonday)
{
    Q_D(WeekDayCondition);
    if (d->setDay(1, onMonday)) {
        emit onMondayChanged();
    }
}

bool WeekDayCondition::isOnTuesday() const
{
    Q_D(const WeekDayCondition);
    return d->checkedDays.contains(2);
}

void WeekDayCondition::setOnTuesday(bool onTuesday)
{
    Q_D(WeekDayCondition);
    if (d->setDay(2, onTuesday)) {
        emit onTuesdayChanged();
    }
}

bool WeekDayCondition::isOnWednesday() const
{
    Q_D(const WeekDayCondition);
    return d->checkedDays.contains(3);
}

void WeekDayCondition::setOnWednesday(bool onWednesday)
{
    Q_D(WeekDayCondition);
    if (d->setDay(3, onWednesday)) {
        emit onWednesdayChanged();
    }
}

bool WeekDayCondition::isOnThursday() const
{
    Q_D(const WeekDayCondition);
    return d->checkedDays.contains(4);
}

void WeekDayCondition::setOnThursday(bool onThursday)
{
    Q_D(WeekDayCondition);
    if (d->setDay(4, onThursday)) {
        emit onThursdayChanged();
    }
}

bool WeekDayCondition::isOnFriday() const
{
    Q_D(const WeekDayCondition);
    return d->checkedDays.contains(5);
}

void WeekDayCondition::setOnFriday(bool onFriday)
{
    Q_D(WeekDayCondition);
    if (d->setDay(5, onFriday)) {
        emit onFridayChanged();
    }
}

bool WeekDayCondition::isOnSaturday() const
{
    Q_D(const WeekDayCondition);
    return d->checkedDays.contains(6);
}

void WeekDayCondition::setOnSaturday(bool onSaturday)
{
    Q_D(WeekDayCondition);
    if (d->setDay(6, onSaturday)) {
        emit onSaturdayChanged();
    }
}

bool WeekDayCondition::isOnSunday() const
{
    Q_D(const WeekDayCondition);
    return d->checkedDays.contains(7);
}

void WeekDayCondition::setOnSunday(bool onSunday)
{
    Q_D(WeekDayCondition);
    if (d->setDay(7, onSunday)) {
        emit onSundayChanged();
    }
}

bool WeekDayCondition::isValid(Rule *rule) const
{
    Q_D(const WeekDayCondition);
    Q_UNUSED(rule);
    int day = QDate::currentDate().dayOfWeek();
    return d->checkedDays.contains(day);
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

QString WeekDayConditionMeta::summary(const QVariantMap &properties) const
{
    QSet<int> checkedDays;

    QStringList days;
    for (int i = 1; i <= 7; ++i) {
        bool checked = properties.value(keyFromDay(i)).toBool();
        if (checked) {
            days.append(QDate::shortDayName(i));
            checkedDays.insert(i);
        }
    }

    if (checkedDays.count() == 5 && checkedDays.contains(1) && checkedDays.contains(2)
        && checkedDays.contains(3) && checkedDays.contains(4) && checkedDays.contains(5)) {
        return tr("On working days");
    }

    if (checkedDays.count() == 2 && checkedDays.contains(6) && checkedDays.contains(7)) {
        return tr("On week-ends");
    }

    if (days.isEmpty()) {
        return name();
    }

    return tr("On %1").arg(days.join(tr(", ")));
}

MetaProperty * WeekDayConditionMeta::getProperty(const QString &property, QObject *parent) const
{
    if (property == ONMONDAY_KEY) {
        return MetaProperty::createBool(property, tr("Monday"), parent);
    }
    if (property == ONTUESDAY_KEY) {
        return MetaProperty::createBool(property, tr("Tuesday"), parent);
    }
    if (property == ONWEDNESDAY_KEY) {
        return MetaProperty::createBool(property, tr("Wednesday"), parent);
    }
    if (property == ONTHURSDAY_KEY) {
        return MetaProperty::createBool(property, tr("Thursday"), parent);
    }
    if (property == ONFRIDAY_KEY) {
        return MetaProperty::createBool(property, tr("Friday"), parent);
    }
    if (property == ONSATURDAY_KEY) {
        return MetaProperty::createBool(property, tr("Saturday"), parent);
    }
    if (property == ONSUNDAY_KEY) {
        return MetaProperty::createBool(property, tr("Sunday"), parent);
    }
    return 0;
}
