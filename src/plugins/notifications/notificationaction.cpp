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

#include "notificationaction.h"

#include "notificationaction.h"
#include <action_p.h>
#include <Notification>

static const char *TEXT_KEY = "text";
static const char *SUMMARY_KEY = "summary";

class NotificationActionPrivate: public ActionPrivate
{
public:
    explicit NotificationActionPrivate(Action *q);
    QString summary;
    QString text;
};

NotificationActionPrivate::NotificationActionPrivate(Action *q)
    : ActionPrivate(q)
{
}

NotificationAction::NotificationAction(QObject *parent)
    : Action(*(new NotificationActionPrivate(this)), parent)
{
}

NotificationAction::~NotificationAction()
{
}

QString NotificationAction::summary() const
{
    Q_D(const NotificationAction);
    return d->summary;
}

void NotificationAction::setSummary(const QString &notificationSummary)
{
    Q_D(NotificationAction);
    if (d->summary != notificationSummary) {
        d->summary = notificationSummary;
        emit summaryChanged();
    }
}

QString NotificationAction::text() const
{
    Q_D(const NotificationAction);
    return d->text;
}

void NotificationAction::setText(const QString &notificationText)
{
    Q_D(NotificationAction);
    if (d->text != notificationText) {
        d->text = notificationText;
        emit textChanged();
    }
}

bool NotificationAction::execute(Rule *rule)
{
    Q_D(NotificationAction);
    Q_UNUSED(rule);

    Notification *notification = new Notification(this);
    notification->setTimestamp(QDateTime::currentDateTime());
    notification->setSummary(d->summary);
    notification->publish();
    notification->deleteLater();
    return true;
}

NotificationActionMeta::NotificationActionMeta(QObject *parent)
    : AbstractMetaData(parent)
{
}

QString NotificationActionMeta::name() const
{
    return tr("Notification");
}

QString NotificationActionMeta::description() const
{
    return tr("This action allows to show a notification when triggered.");
}

QString NotificationActionMeta::summary(const QVariantMap &properties) const
{
    QString summary = properties.value(SUMMARY_KEY).toString();
    QString text = properties.value(TEXT_KEY).toString();

    QString notificationText;
    if (!summary.isEmpty() && text.isEmpty()) {
        notificationText = summary;
    } else if (summary.isEmpty()) {
        notificationText = text;
    } else {
        notificationText = tr("%1, %2").arg(summary, text);
    }

    return tr("Display notification: %1").arg(notificationText);
}

MetaProperty * NotificationActionMeta::getProperty(const QString &property, QObject *parent) const
{
    if (property == SUMMARY_KEY) {
        return MetaProperty::createString(property, tr("Notification Summary"), parent);
    }
    if (property == TEXT_KEY) {
        return MetaProperty::createString(property, tr("Notification Text"), parent);
    }
    return 0;
}
