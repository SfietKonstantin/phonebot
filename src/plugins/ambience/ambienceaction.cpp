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

#include "ambienceaction.h"
#include <action_p.h>
#include <choicemodel.h>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusInterface>

static const char *DBUS_SERVICE = "com.jolla.ambienced";
static const char *DBUS_PATH = "/com/jolla/ambienced";
static const char *DBUS_INTERFACE = "com.jolla.ambienced";
static const char *DBUS_METHOD_NAME = "setAmbience";

class AmbienceActionPrivate: public ActionPrivate
{
public:
    explicit AmbienceActionPrivate(Action *q);

    bool setActiveAmbience(QString ambience);

    QString ambience;
};

AmbienceActionPrivate::AmbienceActionPrivate(Action *q)
    : ActionPrivate(q)
{
}

bool AmbienceActionPrivate::setActiveAmbience(QString ambience)
{
    QDBusConnection connection = QDBusConnection::sessionBus();

    QDBusInterface interface (DBUS_SERVICE, DBUS_PATH, DBUS_INTERFACE, connection);

    // Call the ambience change method, with the path of the desired ambience (format = "file:///xxxxx")
    QDBusMessage result = interface.call(DBUS_METHOD_NAME, ambience);

    if (result.type() == QDBusMessage::ErrorMessage)
    {
        qDebug() << __FUNCTION__ << "result.type() == QDBusMessage::ErrorMessage";
        return false;
    }

    return true;

}

AmbienceAction::AmbienceAction(QObject *parent) :
    Action(*(new AmbienceActionPrivate(this)), parent)
{
}

AmbienceAction::~AmbienceAction()
{
}

QString AmbienceAction::ambience() const
{
    Q_D(const AmbienceAction);
    return d->ambience;
}

void AmbienceAction::setAmbience(const QString &ambience)
{
    Q_D(AmbienceAction);
    if (d->ambience != ambience)
    {
        d->ambience = ambience;
        emit ambienceChanged();
    }
}

bool AmbienceAction::execute(Rule *rule)
{
    Q_D(AmbienceAction);
    Q_UNUSED(rule);

    return d->setActiveAmbience(d->ambience);
}
