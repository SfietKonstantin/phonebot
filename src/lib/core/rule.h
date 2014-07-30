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

#ifndef RULE_H
#define RULE_H

#include <QtCore/QObject>
#include <QtQml/QQmlParserStatus>
#include <QtQml/QQmlListProperty>

class Trigger;
class Condition;
class Action;
class AbstractMapper;
class RulePrivate;
class Rule: public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(Trigger * trigger READ trigger WRITE setTrigger NOTIFY triggerChanged)
    Q_PROPERTY(Condition * condition READ condition WRITE setCondition NOTIFY conditionChanged)
    Q_PROPERTY(QQmlListProperty<Action> actions READ actions)
    Q_PROPERTY(QQmlListProperty<AbstractMapper> mappers READ mappers)
public:
    explicit Rule(QObject *parent = 0);
    virtual ~Rule();
    void classBegin();
    void componentComplete();
    QString name() const;
    void setName(const QString &name);
    bool isEnabled() const;
    void setEnabled(bool enabled);
    Trigger * trigger() const;
    void setTrigger(Trigger *trigger);
    Condition * condition() const;
    void setCondition(Condition *condition);
    QQmlListProperty<Action> actions();
    QQmlListProperty<AbstractMapper> mappers();
Q_SIGNALS:
    void nameChanged();
    void enabledChanged();
    void triggerChanged();
    void conditionChanged();
protected:
    QScopedPointer<RulePrivate> d_ptr;
private:
    Q_DECLARE_PRIVATE(Rule)
    Q_PRIVATE_SLOT(d_func(), void slotTriggered())
};

#endif // RULE_H
