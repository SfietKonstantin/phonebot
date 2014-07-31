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

#ifndef RULEDEFINITION_H
#define RULEDEFINITION_H

#include <QtCore/QObject>
#include <qmldocument.h>
#include "phonebothelper.h"
#include "rulecomponentmodel.h"
#include "ruledefinitionactionmodel.h"

class RuleDefinitionPrivate;
class RuleDefinition : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(RuleComponentModel * trigger READ trigger NOTIFY triggerChanged)
    Q_PROPERTY(RuleComponentModel * condition READ condition NOTIFY conditionChanged)
    Q_PROPERTY(RuleDefinitionActionModel * actions READ actions CONSTANT)
public:
    explicit RuleDefinition(QObject *parent = 0);
    virtual ~RuleDefinition();
    QString name() const;
    void setName(const QString &name);
    RuleComponentModel * trigger() const;
    RuleComponentModel * condition() const;
    RuleDefinitionActionModel * actions() const;
    Q_INVOKABLE RuleComponentModel * createTempComponent(int type, int index, const QString &component);
    Q_INVOKABLE RuleComponentModel * createTempClonedComponent(int type, int index);
    QmlDocumentBase::Ptr toDocument() const;
Q_SIGNALS:
    void nameChanged();
    void triggerChanged();
    void conditionChanged();
public Q_SLOTS:
    void saveComponent(int index);
    void discardComponent(int index);
protected:
    QScopedPointer<RuleDefinitionPrivate> d_ptr;
private:
    Q_DECLARE_PRIVATE(RuleDefinition)
};

#endif // RULEDEFINITION_H
