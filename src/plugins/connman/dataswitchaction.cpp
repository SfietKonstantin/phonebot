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

#include "dataswitchaction.h"
#include "action_p.h"
#include <NetworkManagerFactory>
#include <NetworkManager>
#include <NetworkService>

static const char *ENABLE_KEY = "enable";

class DataSwitchActionPrivate: public ActionPrivate
{
public:
    explicit DataSwitchActionPrivate(Action *q);
    void slotServicesListChanged(const QStringList &servicesList);
    bool enable;
    NetworkManager *networkManager;
    NetworkService *networkService;
private:
    Q_DECLARE_PUBLIC(DataSwitchAction)
};

DataSwitchActionPrivate::DataSwitchActionPrivate(Action *q)
    : ActionPrivate(q)
    , enable(false)
    , networkManager(0)
    , networkService(0)
{
}

void DataSwitchActionPrivate::slotServicesListChanged(const QStringList &servicesList)
{
    Q_UNUSED(servicesList);
    QStringList list = networkManager->servicesList("cellular");
    if (list.count() == 1) {
        networkService->setPath(list.first());
    } else {
        networkService->setPath(QString());;
    }
}

DataSwitchAction::DataSwitchAction(QObject *parent)
    : Action(*(new DataSwitchActionPrivate(this)), parent)
{
    Q_D(DataSwitchAction);
    d->networkManager = NetworkManagerFactory::createInstance();
    d->networkService = new NetworkService(this);
    connect(d->networkManager, SIGNAL(servicesListChanged(const QStringList&)),
            this, SLOT(slotServicesListChanged(const QStringList &)));
}

DataSwitchAction::~DataSwitchAction()
{
    Q_D(DataSwitchAction);
    d->networkService->deleteLater();
}

bool DataSwitchAction::enable() const
{
    Q_D(const DataSwitchAction);
    return d->enable;
}

void DataSwitchAction::setEnable(bool enable)
{
    Q_D(DataSwitchAction);
    if (d->enable != enable) {
        d->enable = enable;
        emit enableChanged();
    }
}

bool DataSwitchAction::execute(Rule *rule, QString &error)
{
    Q_UNUSED(rule);
    Q_D(DataSwitchAction);
    qDebug() << "Data path:" << d->networkService->path();
    if (d->networkService->path().isEmpty()) {
        error = tr("Technical issue: failed to find the data network service");
        return false;
    }

    qDebug() << "Data favorite:" << d->networkService->favorite();
    qDebug() << "Data auto-connect:" << d->networkService->autoConnect();
    if (!d->networkService->favorite()) {
        error = tr("Technical issue: the data network service is not in the favorites");
        return false;
    }

    d->networkService->setAutoConnect(d->enable);
    return true;
}

DataSwitchActionMeta::DataSwitchActionMeta(QObject *parent)
    : AbstractMetaData(parent)
{
}

QString DataSwitchActionMeta::name() const
{
    return tr("Data switch");
}

QString DataSwitchActionMeta::description() const
{
    return tr("This action allows to enable / disable data.");
}

QString DataSwitchActionMeta::summary(const QVariantMap &properties) const
{
    bool enable = properties.value(ENABLE_KEY).toBool();
    if (enable) {
        return tr("Enable data");
    } else {
        return tr("Disable data");
    }
}

MetaProperty * DataSwitchActionMeta::getProperty(const QString &property, QObject *parent) const
{
    if (property == ENABLE_KEY) {
        return MetaProperty::create(property, MetaProperty::Bool, tr("Switch data"), parent);
    }
    return 0;
}

#include "moc_dataswitchaction.cpp"
