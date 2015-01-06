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

#ifndef TIMETRIGGER_H
#define TIMETRIGGER_H

#include <trigger.h>
#include <abstractmetadata.h>
#include <QtCore/QTime>

class TimeTriggerPrivate;
class TimeTrigger : public Trigger
{
    Q_OBJECT
    Q_PROPERTY(QTime time READ time WRITE setTime NOTIFY timeChanged)
    PHONEBOT_METADATA(TimeTriggerMeta)
public:
    explicit TimeTrigger(QObject *parent = 0);
    virtual ~TimeTrigger();
    QTime time() const;
    void setTime(const QTime &time);
Q_SIGNALS:
    void timeChanged();
private:
    Q_DECLARE_PRIVATE(TimeTrigger)
    Q_PRIVATE_SLOT(d_func(), void slotTriggered())
    Q_PRIVATE_SLOT(d_func(), void slotTimerTriggered())
};

class TimeTriggerMeta: public AbstractMetaData
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit TimeTriggerMeta(QObject * parent = 0);
    QString name() const;
    QString description() const;
    QString summary(const QVariantMap &properties) const;
protected:
    MetaProperty * getProperty(const QString &property, QObject *parent = 0) const;
};

#endif // TIMETRIGGER_H
