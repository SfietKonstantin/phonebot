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

#include "debugtrigger.h"
#include <trigger_p.h>
#include <QtCore/QDebug>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusInterface>
#include "adaptor.h"

static const char *DBUS_SERVICE = "org.freedesktop.DBus";
static const char *DBUS_PATH = "/org/freedesktop/DBus";
static const char *DBUS_INTERFACE = "org.freedesktop.DBus";
static const char *DBUS_LIST_NAMES = "ListNames";

static const char *SERVICE = "org.SfietKonstantin.phonebotdebug";

static const char *ROOT = "/";
static const char *PING = "Ping";
static const char *UNKNOWN_OBJECT = "org.freedesktop.DBus.Error.UnknownObject";

class DebugTriggerPrivate: public TriggerPrivate
{
public:
    explicit DebugTriggerPrivate(Trigger *q);
    bool registerToBus();
    void unregisterFromBus();
    QString registeredPath;
    QString path;
    bool registered;
private:
    Q_DECLARE_PUBLIC(DebugTrigger)
};

DebugTriggerPrivate::DebugTriggerPrivate(Trigger *q)
    : TriggerPrivate(q), registered(false)
{
}

bool DebugTriggerPrivate::registerToBus()
{
    Q_Q(DebugTrigger);
    QDBusConnection connection = QDBusConnection::sessionBus();

    // Check if we are already registered
    QDBusInterface interface (DBUS_SERVICE, DBUS_PATH, DBUS_INTERFACE, connection);
    QDBusMessage result = interface.call(DBUS_LIST_NAMES);
    if (result.type() == QDBusMessage::ErrorMessage) {
        return false;
    }

    QList<QVariant> arguments = result.arguments();
    if (arguments.count() != 1) {
        return false;
    }

    QStringList interfaces = arguments.first().toStringList();
    if (!interfaces.contains(SERVICE)) {
        connection.registerService(SERVICE);
    }

    // Register this object
    registeredPath = path.trimmed();
    return connection.registerObject(registeredPath, q);
}

void DebugTriggerPrivate::unregisterFromBus()
{
    QDBusConnection connection = QDBusConnection::sessionBus();
    connection.unregisterObject(registeredPath);

    QDBusInterface interface (SERVICE, ROOT, DBUS_INTERFACE);
    QDBusMessage result = interface.call(PING);
    if (result.type() == QDBusMessage::ErrorMessage) {
        if (result.errorName() == UNKNOWN_OBJECT) {
            connection.unregisterService(SERVICE);
        }
    }
}

DebugTrigger::DebugTrigger(QObject *parent) :
    Trigger(*(new DebugTriggerPrivate(this)), parent)
{
    new PhonebotdebugAdaptor(this);
}

DebugTrigger::~DebugTrigger()
{
    Q_D(DebugTrigger);
    d->unregisterFromBus();
}

void DebugTrigger::Ping()
{
    emit triggered();
}

QString DebugTrigger::path() const
{
    Q_D(const DebugTrigger);
    return d->path;
}

void DebugTrigger::setPath(const QString &path)
{
    Q_D(DebugTrigger);
    if (d->path != path) {
        d->path = path;
        emit pathChanged();
    }
}

void DebugTrigger::componentComplete()
{
    Q_D(DebugTrigger);
    if (!d->path.isEmpty() && d->path.trimmed() != "/") {
        d->registerToBus();
    }
}
