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

#ifndef WLANSWITCHACTION_H
#define WLANSWITCHACTION_H

#include <action.h>
#include <abstractmetadata.h>

class WlanSwitchActionPrivate;
class WlanSwitchAction : public Action
{
    Q_OBJECT
    Q_PROPERTY(bool enable READ enable WRITE setEnable NOTIFY enableChanged)
    PHONEBOT_METADATA(WlanSwitchActionMeta)
public:
    explicit WlanSwitchAction(QObject *parent = 0);
    virtual ~WlanSwitchAction();
    bool enable() const;
    void setEnable(bool enable);
    bool execute(Rule *rule);
Q_SIGNALS:
    void enableChanged();
private:
    Q_DECLARE_PRIVATE(WlanSwitchAction)
    Q_PRIVATE_SLOT(d_func(), void slotTechnologiesChanged())
};

class WlanSwitchActionMeta: public AbstractMetaData
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit WlanSwitchActionMeta(QObject * parent = 0);
    QString name() const;
    QString description() const;
    QString summary(const QVariantMap &properties) const;
protected:
    MetaProperty * getProperty(const QString &property, QObject *parent = 0) const;
};

#endif // WLANSWITCHACTION_H
