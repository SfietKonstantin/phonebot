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

#include "jscondition.h"
#include "condition_p.h"
#include <QtQml/QQmlContext>
#include <QtQml/QQmlEngine>
#include "rule.h"

class JsConditionPrivate: public ConditionPrivate
{
public:
    explicit JsConditionPrivate(Condition *q);
    QJSValue condition;
private:
    Q_DECLARE_PUBLIC(JsCondition)
};

JsConditionPrivate::JsConditionPrivate(Condition *q)
    : ConditionPrivate(q)
{
}

JsCondition::JsCondition(QObject *parent) :
    Condition(*(new JsConditionPrivate(this)), parent)
{
}

QJSValue JsCondition::condition() const
{
    Q_D(const JsCondition);
    return d->condition;
}

void JsCondition::setCondition(const QJSValue &condition)
{
    Q_D(JsCondition);
    if (!d->condition.strictlyEquals(condition)) {
        d->condition = condition;
        emit conditionChanged();
    }
}

bool JsCondition::isValid(Rule *rule)
{
    Q_D(JsCondition);
    if (!d->condition.isCallable()) {
        return false;
    }

    QQmlContext *context = QQmlEngine::contextForObject(this);
    if (!context) {
        return false;
    }

    QQmlEngine *engine = context->engine();
    if (!engine) {
        return false;
    }

    QJSValueList args;
    args.append(engine->newQObject(rule));
    engine->setObjectOwnership(rule, QQmlEngine::CppOwnership);
    QJSValue returned = d->condition.call(args);
    bool ok = false;
    if (returned.isBool()) {
        ok = returned.toBool();
    }
    return ok;
}
