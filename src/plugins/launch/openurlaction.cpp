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

#include "openurlaction.h"
#include "action_p.h"
#include <QtGui/QDesktopServices>

static const char *URL_KEY = "url";

class OpenUrlActionPrivate: public ActionPrivate
{
public:
    explicit OpenUrlActionPrivate(Action *q);
    QString url;
private:
    Q_DECLARE_PUBLIC(OpenUrlAction)
};

OpenUrlActionPrivate::OpenUrlActionPrivate(Action *q)
    : ActionPrivate(q)
{
}

OpenUrlAction::OpenUrlAction(QObject *parent)
    : Action(*(new OpenUrlActionPrivate(this)), parent)
{
}

OpenUrlAction::~OpenUrlAction()
{
}

QString OpenUrlAction::url() const
{
    Q_D(const OpenUrlAction);
    return d->url;
}

void OpenUrlAction::setUrl(const QString &url)
{
    Q_D(OpenUrlAction);
    if (d->url != url) {
        d->url = url;
        emit urlChanged();
    }
}

bool OpenUrlAction::execute(Rule *rule)
{
    Q_UNUSED(rule);
    Q_D(OpenUrlAction);
    qDebug() << "Opening url:" << d->url;
    return QDesktopServices::openUrl(QUrl(d->url));
}

OpenUrlActionMeta::OpenUrlActionMeta(QObject *parent)
    : AbstractMetaData(parent)
{
}

QString OpenUrlActionMeta::name() const
{
    return tr("Open URL");
}

QString OpenUrlActionMeta::description() const
{
    return tr("This action allows opening an URL in the default browser.");
}

QString OpenUrlActionMeta::summary(const QVariantMap &properties) const
{
    QString url = properties.value(URL_KEY).toString();
    if (url.isEmpty()) {
        return QString();
    }
    return tr("Open url %1").arg(url);
}

MetaProperty * OpenUrlActionMeta::getProperty(const QString &property, QObject *parent) const
{
    if (property == URL_KEY) {
        return MetaProperty::createString(property, tr("Url to open"), parent);
    }
    return 0;
}
