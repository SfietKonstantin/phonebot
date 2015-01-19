/*
 * Copyright (C) 2015 Zeta Sagittarii <zeta@sagittarii.fr>
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
#include <action_p.h>
#include <choicemodel.h>
//#include <QtDBus/QDBusConnection>
//#include <QtDBus/QDBusInterface>
#include <QtDBus>

static const char *SUMMARY_KEY = "notificationSummary";
static const char *TEXT_KEY = "notificationText";

class NotificationActionPrivate: public ActionPrivate
{
public:
    explicit NotificationActionPrivate(Action *q);

    QString notificationText;
    QString notificationSummary;

private:
};

NotificationActionPrivate::NotificationActionPrivate(Action *q)
    : ActionPrivate(q)
{
}

NotificationAction::NotificationAction(QObject *parent) :
    Action(*(new NotificationActionPrivate(this)), parent)
{
}

NotificationAction::~NotificationAction()
{
}

QString NotificationAction::notificationText() const
{
    Q_D(const NotificationAction);
    return d->notificationText;
}

void NotificationAction::setNotificationText(const QString &notificationText)
{
    Q_D(NotificationAction);
    if (d->notificationText != notificationText)
    {
        d->notificationText = notificationText;
        emit notificationTextChanged();
    }
}

QString NotificationAction::notificationSummary() const
{
    Q_D(const NotificationAction);
    return d->notificationSummary;
}

void NotificationAction::setNotificationSummary(const QString &notificationSummary)
{
    Q_D(NotificationAction);
    if (d->notificationSummary != notificationSummary)
    {
        d->notificationSummary = notificationSummary;
        emit notificationSummaryChanged();
    }
}


bool NotificationAction::execute(Rule *rule)
{
    Q_D(NotificationAction);
    Q_UNUSED(rule);

    sendFreedesktopNotification(d->notificationSummary, d->notificationText);

    return true;
}


void NotificationAction::sendFreedesktopNotification(QString summary,
                                                   QString text)
{
    qDebug() << __FUNCTION__ << summary << text;

    QDBusInterface* dbusNotificationInterface =
            new QDBusInterface("org.freedesktop.Notifications",
                               "/org/freedesktop/Notifications",
                               "org.freedesktop.Notifications",
                               QDBusConnection::sessionBus(),
                               this);

    if (!dbusNotificationInterface->isValid())
    {
        qDebug() << "Unable to to connect to org.freedesktop.Notifications dbus service.";
        dbusNotificationInterface = NULL;
    }

    if (dbusNotificationInterface && dbusNotificationInterface->isValid())
    {
        QString appName = "PhoneBot";

        /// \note see https://developer.gnome.org/notification-spec/
        QList<QVariant> args;
        args.append(appName); // Application Name
        args.append(0U); // Replaces ID (0U)
        //args.append(QString("/usr/share/icons/hicolor/86x86/apps/harbour-phonebot.png")); // Notification Icon
        args.append(QString("")); // Notification Icon
        args.append(summary); // Summary
        args.append(text); // Body
        args.append(QStringList()); // Actions

        QVariantMap hints;
        // for hints to make icon, see
        // https://dev.visucore.com/bitcoin/doxygen/notificator_8cpp_source.html
        args.append(hints);
        args.append(0);

        qDebug() << args;

        QDBusMessage msg = dbusNotificationInterface->callWithArgumentList(QDBus::NoBlock,
                                                        "Notify", args);
        qDebug() << "Call returned : " << msg;
    }
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

    return properties.value(SUMMARY_KEY).toString() + " / " + properties.value(TEXT_KEY).toString();
}

MetaProperty * NotificationActionMeta::getProperty(const QString &property, QObject *parent) const
{
    if (property == SUMMARY_KEY) {
        return MetaProperty::create(property, MetaProperty::String, tr("Notification Summary"), parent);
    }
    if (property == TEXT_KEY) {
        return MetaProperty::create(property, MetaProperty::String, tr("Notification Text"), parent);
    }
    return 0;
}

