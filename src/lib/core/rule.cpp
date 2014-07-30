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

#include "rule.h"
#include "rule_p.h"
#include "action.h"
#include "condition.h"
#include "trigger.h"

RulePrivate::RulePrivate(Rule *q)
    : enabled(true), trigger(0), condition(0), q_ptr(q)
{
}

void RulePrivate::actions_append(QQmlListProperty<Action> *list, Action *action)
{
    Rule *rule = qobject_cast<Rule *>(list->object);
    if (rule && action) {
        rule->d_func()->actions.append(action);
    }
}

Action * RulePrivate::actions_at(QQmlListProperty<Action> *list, int index)
{
    Rule *rule = qobject_cast<Rule *>(list->object);
    if (rule && index >= 0 && index < rule->d_func()->actions.count()) {
        return rule->d_func()->actions.at(index);
    }
    return 0;
}

void RulePrivate::actions_clear(QQmlListProperty<Action> *list)
{
    Rule *rule = qobject_cast<Rule *>(list->object);
    if (rule) {
        rule->d_func()->actions.clear();
    }
}

int RulePrivate::actions_count(QQmlListProperty<Action> *list)
{
    Rule *rule = qobject_cast<Rule *>(list->object);
    if (rule) {
        return rule->d_func()->actions.count();
    }
    return 0;
}

void RulePrivate::mappers_append(QQmlListProperty<AbstractMapper> *list, AbstractMapper *mapper)
{
    Rule *rule = qobject_cast<Rule *>(list->object);
    if (rule && mapper) {
        rule->d_func()->mappers.append(mapper);
    }
}

AbstractMapper * RulePrivate::mappers_at(QQmlListProperty<AbstractMapper> *list, int index)
{
    Rule *rule = qobject_cast<Rule *>(list->object);
    if (rule && index >= 0 && index < rule->d_func()->mappers.count()) {
        return rule->d_func()->mappers.at(index);
    }
    return 0;
}

void RulePrivate::mappers_clear(QQmlListProperty<AbstractMapper> *list)
{
    Rule *rule = qobject_cast<Rule *>(list->object);
    if (rule) {
        rule->d_func()->mappers.clear();
    }
}

int RulePrivate::mappers_count(QQmlListProperty<AbstractMapper> *list)
{
    Rule *rule = qobject_cast<Rule *>(list->object);
    if (rule) {
        return rule->d_func()->mappers.count();
    }
    return 0;
}

void RulePrivate::slotTriggered()
{
    Q_Q(Rule);
    if (!enabled) {
        return;
    }

    if (!trigger) {
        return;
    }

    bool ok = true;
    if (condition) {
        ok = condition->isEnabled();
        if (ok) {
            ok = condition->isValid(q);
        }
    }

    if (ok) {
        foreach (Action *action, actions) {
            action->execute(q);
        }
    }
}

Rule::Rule(QObject *parent)
    : QObject(parent), d_ptr(new RulePrivate(this))
{
}

Rule::~Rule()
{
}

void Rule::classBegin()
{
}

void Rule::componentComplete()
{
}

QString Rule::name() const
{
    Q_D(const Rule);
    return d->name;
}

void Rule::setName(const QString &name)
{
    Q_D(Rule);
    if (d->name != name) {
        d->name = name;
        emit nameChanged();
    }
}

bool Rule::isEnabled() const
{
    Q_D(const Rule);
    return d->enabled;
}

void Rule::setEnabled(bool enabled)
{
    Q_D(Rule);
    if (d->enabled != enabled) {
        d->enabled = enabled;
        emit enabledChanged();
    }
}

Trigger * Rule::trigger() const
{
    Q_D(const Rule);
    return d->trigger;
}

void Rule::setTrigger(Trigger *trigger)
{
    Q_D(Rule);
    if (d->trigger != trigger) {
        if (d->trigger) {
            disconnect(d->trigger, SIGNAL(triggered()), this, SLOT(slotTriggered));
        }
        d->trigger = trigger;
        if (d->trigger) {
            connect(d->trigger, SIGNAL(triggered()), this, SLOT(slotTriggered()));
        }
        emit triggerChanged();
    }
}

Condition * Rule::condition() const
{
    Q_D(const Rule);
    return d->condition;
}

void Rule::setCondition(Condition *condition)
{
    Q_D(Rule);
    if (d->condition != condition) {
        d->condition = condition;
        emit conditionChanged();
    }
}

QQmlListProperty<Action> Rule::actions()
{
    return QQmlListProperty<Action>(this, 0,
                                    &RulePrivate::actions_append,
                                    &RulePrivate::actions_count,
                                    &RulePrivate::actions_at,
                                    &RulePrivate::actions_clear);
}

QQmlListProperty<AbstractMapper> Rule::mappers()
{
    return QQmlListProperty<AbstractMapper>(this, 0,
                                            &RulePrivate::mappers_append,
                                            &RulePrivate::mappers_count,
                                            &RulePrivate::mappers_at,
                                            &RulePrivate::mappers_clear);
}

#include "moc_rule.cpp"
