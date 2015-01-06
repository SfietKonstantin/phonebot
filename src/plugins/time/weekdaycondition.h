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

#ifndef WEEKDAYCONDITION_H
#define WEEKDAYCONDITION_H

#include <condition.h>
#include <abstractmetadata.h>

class WeekDayConditionPrivate;
class WeekDayCondition : public Condition
{
    Q_OBJECT
    Q_PROPERTY(bool onMonday READ isOnMonday WRITE setOnMonday NOTIFY onMondayChanged)
    Q_PROPERTY(bool onTuesday READ isOnTuesday WRITE setOnTuesday NOTIFY onTuesdayChanged)
    Q_PROPERTY(bool onWednesday READ isOnWednesday WRITE setOnWednesday NOTIFY onWednesdayChanged)
    Q_PROPERTY(bool onThursday READ isOnThursday WRITE setOnThursday NOTIFY onThursdayChanged)
    Q_PROPERTY(bool onFriday READ isOnFriday WRITE setOnFriday NOTIFY onFridayChanged)
    Q_PROPERTY(bool onSaturday READ isOnSaturday WRITE setOnSaturday NOTIFY onSaturdayChanged)
    Q_PROPERTY(bool onSunday READ isOnSunday WRITE setOnSunday NOTIFY onSundayChanged)
    PHONEBOT_METADATA(WeekDayConditionMeta)
public:
    explicit WeekDayCondition(QObject *parent = 0);
    bool isOnMonday() const;
    void setOnMonday(bool onMonday);
    bool isOnTuesday() const;
    void setOnTuesday(bool onTuesday);
    bool isOnWednesday() const;
    void setOnWednesday(bool onWednesday);
    bool isOnThursday() const;
    void setOnThursday(bool onThursday);
    bool isOnFriday() const;
    void setOnFriday(bool onFriday);
    bool isOnSaturday() const;
    void setOnSaturday(bool onSaturday);
    bool isOnSunday() const;
    void setOnSunday(bool onSunday);
    bool isValid(Rule *rule) const override;
Q_SIGNALS:
    void onMondayChanged();
    void onTuesdayChanged();
    void onWednesdayChanged();
    void onThursdayChanged();
    void onFridayChanged();
    void onSaturdayChanged();
    void onSundayChanged();
private:
    Q_DECLARE_PRIVATE(WeekDayCondition)
};

class WeekDayConditionMeta: public AbstractMetaData
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit WeekDayConditionMeta(QObject * parent = 0);
    QString name() const override;
    QString description() const override;
    QString summary(const QVariantMap &properties) const override;
protected:
    MetaProperty * getProperty(const QString &property, QObject *parent = 0) const override;
};

#endif // WEEKDAYCONDITION_H
