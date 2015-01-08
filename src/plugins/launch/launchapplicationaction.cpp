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

#include "launchapplicationaction.h"
#include "action_p.h"
#include <QtCore/QProcess>
#include <MDesktopEntry>

static const char *DESKTOP_FILE_KEY = "desktopFile";

class LaunchApplicationActionPrivate: public ActionPrivate
{
public:
    explicit LaunchApplicationActionPrivate(Action *q);
    MDesktopEntry *desktopEntry;
private:
    Q_DECLARE_PUBLIC(LaunchApplicationAction)
};

LaunchApplicationActionPrivate::LaunchApplicationActionPrivate(Action *q)
    : ActionPrivate(q), desktopEntry(0)
{
}

LaunchApplicationAction::LaunchApplicationAction(QObject *parent)
    : Action(*(new LaunchApplicationActionPrivate(this)), parent)
{
}

LaunchApplicationAction::~LaunchApplicationAction()
{
    Q_D(LaunchApplicationAction);
    if (d->desktopEntry) {
        delete d->desktopEntry;
    }
}

QString LaunchApplicationAction::desktopFile() const
{
    Q_D(const LaunchApplicationAction);
    return d->desktopEntry ? d->desktopEntry->fileName() : QString();
}

void LaunchApplicationAction::setDesktopFile(const QString &desktopFile)
{
    Q_D(LaunchApplicationAction);
    if (this->desktopFile() != desktopFile) {
        if (d->desktopEntry) {
            delete d->desktopEntry;
        }
        d->desktopEntry = new MDesktopEntry(desktopFile);
        emit desktopFileChanged();
    }
}

bool LaunchApplicationAction::execute(Rule *rule)
{
    Q_UNUSED(rule);
    Q_D(LaunchApplicationAction);

    if (!d->desktopEntry) {
        qDebug() << "No application set";
        return false;
    }

    QString application = d->desktopEntry->exec();
    // Based on contentaction
    if (d->desktopEntry->terminal()) {
        qDebug() << "Cannot run terminal-based application";
        return false;
    }

    if (!application.startsWith("invoker") && !application.startsWith("/usr/bin/invoker")) {
        QString boosterType = d->desktopEntry->value("Desktop Entry", "X-Nemo-Application-Type");
        if (boosterType.isNull()) {
            boosterType = "generic";
        }

        bool singleInstance = true;
        QString singleInstanceValue = d->desktopEntry->value("Desktop Entry", "X-Nemo-Single-Instance");
        if (singleInstanceValue == "no") {
            singleInstance = false;
        }

        if (boosterType != "no-invoker") {
            application.prepend(QString("invoker --type=%1 %2").arg(boosterType, singleInstance ? "--single-instance" : ""));
        }
    }

    qDebug() << "Launching application via" << d->desktopEntry->exec();
    QProcess::startDetached(application);
    return true;
}

LaunchApplicationActionMeta::LaunchApplicationActionMeta(QObject *parent)
    : AbstractMetaData(parent)
{
}

QString LaunchApplicationActionMeta::name() const
{
    return tr("Launch application");
}

QString LaunchApplicationActionMeta::description() const
{
    return tr("This action allows launching an application.");
}

QString LaunchApplicationActionMeta::summary(const QVariantMap &properties) const
{
    QString desktopFile = properties.value(DESKTOP_FILE_KEY).toString();
    if (desktopFile.isEmpty()) {
        return QString();
    }

    MDesktopEntry desktopEntry (desktopFile);
    if (!desktopEntry.isValid()) {
        return QString();
    }

    return tr("Launch %1").arg(desktopEntry.name());
}

MetaProperty * LaunchApplicationActionMeta::getProperty(const QString &property, QObject *parent) const
{
    if (property == DESKTOP_FILE_KEY) {
        return MetaProperty::createApplication(property, tr("Application to launch"), parent);
    }
    return 0;
}
