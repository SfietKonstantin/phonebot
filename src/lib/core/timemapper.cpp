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

#include "timemapper.h"
#include "abstractmapper_p.h"
#include <QtCore/QTime>

class TimeMapperPrivate: public AbstractMapperPrivate
{
public:
    explicit TimeMapperPrivate(TimeMapper *q);
    void setTime();
    int hour;
    int minute;
private:
    Q_DECLARE_PUBLIC(TimeMapper)
};

TimeMapperPrivate::TimeMapperPrivate(TimeMapper *q)
    : AbstractMapperPrivate(q)
    , hour(-1)
    , minute(-1)
{
}

void TimeMapperPrivate::setTime()
{
    Q_Q(TimeMapper);
    if (hour >= 0 && hour < 24 && minute >= 0 && minute < 60) {
        q->setValue(QTime(hour, minute));
    } else {
        q->setValue(QTime());
    }
}

TimeMapper::TimeMapper(QObject *parent) :
    AbstractMapper(*(new TimeMapperPrivate(this)), parent)
{
}

int TimeMapper::hour() const
{
    Q_D(const TimeMapper);
    return d->hour;
}

void TimeMapper::setHour(int hour)
{
    Q_D(TimeMapper);
    if (d->hour != hour) {
        d->hour = hour;
        d->setTime();
        emit hourChanged();
    }
}

int TimeMapper::minute() const
{
    Q_D(const TimeMapper);
    return d->minute;
}

void TimeMapper::setMinute(int minute)
{
    Q_D(TimeMapper);
    if (d->minute != minute) {
        d->minute = minute;
        d->setTime();
        emit minuteChanged();
    }
}

