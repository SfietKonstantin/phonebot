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

#include "wlanswitchaction.h"
#include "action_p.h"
#include <NetworkManagerFactory>
#include <NetworkManager>
#include <NetworkTechnology>

static const char *ENABLE_KEY = "enable";

class WlanSwitchActionPrivate: public ActionPrivate
{
public:
    explicit WlanSwitchActionPrivate(Action *q);
    void slotTechnologiesChanged();
    bool enable;
    NetworkTechnology *wifiTechnology;
private:
    Q_DECLARE_PUBLIC(WlanSwitchAction)
};

WlanSwitchActionPrivate::WlanSwitchActionPrivate(Action *q)
    : ActionPrivate(q)
    , enable(false)
    , wifiTechnology(0)
{
}

void WlanSwitchActionPrivate::slotTechnologiesChanged()
{
    wifiTechnology->setPath(NetworkManagerFactory::createInstance()->technologyPathForType("wifi"));
}

WlanSwitchAction::WlanSwitchAction(QObject *parent)
    : Action(*(new WlanSwitchActionPrivate(this)), parent)
{
    Q_D(WlanSwitchAction);
    NetworkManager *networkManager = NetworkManagerFactory::createInstance();
    d->wifiTechnology = new NetworkTechnology(this);
    d->wifiTechnology->setPath(networkManager->technologyPathForType("wifi"));
    connect(networkManager, SIGNAL(technologiesChanged()), this, SLOT(slotTechnologiesChanged()));
    connect(networkManager, SIGNAL(technologiesEnabledChanged()), this, SLOT(slotTechnologiesChanged()));
}

WlanSwitchAction::~WlanSwitchAction()
{
}

bool WlanSwitchAction::enable() const
{
    Q_D(const WlanSwitchAction);
    return d->enable;
}

void WlanSwitchAction::setEnable(bool enable)
{
    Q_D(WlanSwitchAction);
    if (d->enable != enable) {
        d->enable = enable;
        emit enableChanged();
    }
}

bool WlanSwitchAction::execute(Rule *rule)
{
    Q_UNUSED(rule);
    Q_D(WlanSwitchAction);
    qDebug() << "Wlan path:" << d->wifiTechnology->path();
    if (d->wifiTechnology->path().isEmpty()) {
        return false;
    }

    qDebug() << "Wlan tethering:" << d->wifiTechnology->tethering();
    qDebug() << "Wlan powered:" << d->wifiTechnology->powered();

    // Don't interrupt tethering
    if (d->wifiTechnology->tethering()) {
        return false;
    }

    d->wifiTechnology->setPowered(d->enable);
    return true;
}

WlanSwitchActionMeta::WlanSwitchActionMeta(QObject *parent)
    : AbstractMetaData(parent)
{
}

QString WlanSwitchActionMeta::name() const
{
    return tr("WLAN switch");
}

QString WlanSwitchActionMeta::description() const
{
    return tr("This action allows to enable / disable WLAN.");
}

QString WlanSwitchActionMeta::summary(const QVariantMap &properties) const
{
    bool enable = properties.value(ENABLE_KEY).toBool();
    if (enable) {
        return tr("Enable WLAN");
    } else {
        return tr("Disable WLAN");
    }
}

MetaProperty * WlanSwitchActionMeta::getProperty(const QString &property, QObject *parent) const
{
    if (property == ENABLE_KEY) {
        return MetaProperty::createBool(property, tr("Switch WLAN"), parent);
    }
    return 0;
}

#include "moc_wlanswitchaction.cpp"

