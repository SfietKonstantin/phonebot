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

#include "timetrigger.h"
#include "trigger_p.h"
#include <QtCore/QDate>
#include <QtCore/QDebug>
#include <KeepAlive>

static const char *TIME_KEY = "time";
static const int DELTA = 75000;

class TimeTriggerPrivate: public TriggerPrivate
{
public:
    explicit TimeTriggerPrivate(Trigger *q);
    void slotTriggered();
    QTime time;
    QDate lastEmission;
    DeclarativeBackgroundJob *backgroundJob;
private:
    Q_DECLARE_PUBLIC(TimeTrigger)
};

TimeTriggerPrivate::TimeTriggerPrivate(Trigger *q)
    : TriggerPrivate(q), backgroundJob(0)
{
}

void TimeTriggerPrivate::slotTriggered()
{
    Q_Q(TimeTrigger);
    backgroundJob->begin();

    int delta = QTime::currentTime().msecsTo(time);
    if (delta <= DELTA && delta > -DELTA) {
        if (lastEmission != QDate::currentDate()) {
            lastEmission = QDate::currentDate(); // Ensure that the signal is emitted once per day
            qDebug() << "Triggered time:" << QTime::currentTime();
            emit q->triggered();
        }
    }

    backgroundJob->finished();
}

TimeTrigger::~TimeTrigger()
{
    Q_D(TimeTrigger);
    d->backgroundJob->setEnabled(false);
}

TimeTrigger::TimeTrigger(QObject *parent) :
    Trigger(*(new TimeTriggerPrivate(this)), parent)
{
    Q_D(TimeTrigger);
    d->backgroundJob = new DeclarativeBackgroundJob(this);
    d->backgroundJob->classBegin();
    d->backgroundJob->componentComplete();
    d->backgroundJob->setFrequency(DeclarativeBackgroundJob::TwoAndHalfMinutes);
    connect(d->backgroundJob, SIGNAL(triggered()), this, SLOT(slotTriggered()));
    d->backgroundJob->setEnabled(true);
}

QTime TimeTrigger::time() const
{
    Q_D(const TimeTrigger);
    return d->time;
}

void TimeTrigger::setTime(const QTime &time)
{
    Q_D(TimeTrigger);
    if (d->time != time) {
        d->time = time;
        qDebug() << "Time set:" << time;
        emit timeChanged();
    }
}

TimeTriggerMeta::TimeTriggerMeta(QObject *parent)
    : AbstractMetaData(parent)
{
}

QString TimeTriggerMeta::name() const
{
    return tr("Time");
}

QString TimeTriggerMeta::description() const
{
    return tr("This trigger will be triggered at a specific time.");
}

QString TimeTriggerMeta::summary(const QVariantMap &properties) const
{
    QTime time = properties.value(TIME_KEY).toTime();
    if (time.isValid()) {
        return tr("When it is %1").arg(time.toString(tr("hh:mm")));
    }
    return name();
}

MetaProperty * TimeTriggerMeta::getProperty(const QString &property, QObject *parent) const
{
    if (property == TIME_KEY) {
        return MetaProperty::create(property, MetaProperty::Time, tr("Time of day to trigger"),
                                    parent);
    }
    return 0;
}

#include "moc_timetrigger.cpp"

