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

#include "condition.h"
#include "condition_p.h"

ConditionPrivate::ConditionPrivate(Condition *q)
    : valid(true), enabled(true), q_ptr(q)
{
}

Condition::Condition(QObject *parent)
    : QObject(parent), d_ptr(new ConditionPrivate(this))
{
}

Condition::Condition(ConditionPrivate &dd, QObject *parent)
    : QObject(parent), d_ptr(&dd)
{
}

Condition::~Condition()
{
}

void Condition::classBegin()
{
}

void Condition::componentComplete()
{
}

bool Condition::isEnabled() const
{
    Q_D(const Condition);
    return d->enabled;
}

void Condition::setEnabled(bool enabled)
{
    Q_D(Condition);
    if (d->enabled != enabled) {
        d->enabled = enabled;
        emit enabledChanged();
    }
}

bool Condition::isValid() const
{
    Q_D(const Condition);
    return d->valid;
}

void Condition::setValid(bool valid)
{
    Q_D(Condition);
    if (d->valid != valid) {
        d->valid = valid;
        emit validChanged();
    }
}
