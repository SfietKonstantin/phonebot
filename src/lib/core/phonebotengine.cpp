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

#include "phonebotengine.h"
#include "phonebotengine_p.h"
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QPluginLoader>
#include <QtCore/QSet>
#include <QtQml/qqml.h>
#include "action.h"
#include "jsaction.h"
#include "condition.h"
#include "jscondition.h"
#include "phonebotextensionplugin.h"
#include "rule.h"
#include "timemapper.h"
#include "trigger.h"

static const char *REASON = "Cannot be created";

PhoneBotEnginePrivate::PhoneBotEnginePrivate(PhoneBotEngine *q)
    : q_ptr(q)
{
}

void PhoneBotEnginePrivate::slotComponentFinished(QQmlComponent::Status status)
{
    Q_Q(PhoneBotEngine);
    Q_UNUSED(status)
    manageComponentFinished(qobject_cast<QQmlComponent *>(q->sender()));
}

void PhoneBotEnginePrivate::manageComponentFinished(QQmlComponent *component)
{
    Q_Q(PhoneBotEngine);
    if (!component) {
        return;
    }
    bool ok = component->isReady();
    QUrl url = component->url();
    if (ok) {
        components.insert(url, component);
    } else {
        if (component->isError()) {
            QString error = component->errorString();
            qWarning() << error;
            componentErrors.insert(url, error);
            component->deleteLater();
        }
    }
    emit q->componentLoadingFinished(url, ok);
}

void PhoneBotEnginePrivate::setRuleError(const QUrl &url, const QString &error)
{
    ruleErrors.insert(url, error);
    qWarning() << error;
}

bool PhoneBotEnginePrivate::checkRule(Rule *rule)
{
    if (!rule) {
        return false;
    }

    // Check if the rule have a trigger and an action
    QQmlListReference reference (rule, "actions");
    if (!rule->trigger() || reference.count() == 0) {
        return false;
    }

    return true;
}

void PhoneBotEnginePrivate::deleteRule(Rule *rule)
{
    if (QQmlEngine::objectOwnership(rule) == QQmlEngine::CppOwnership) {
        rule->deleteLater();
    }
}

PhoneBotEngine::PhoneBotEngine(QObject *parent)
    : QQmlEngine(parent), d_ptr(new PhoneBotEnginePrivate(this))
{
}

PhoneBotEngine::~PhoneBotEngine()
{
    stop();
}

void PhoneBotEngine::registerTypes()
{
    qmlRegisterType<Trigger>("org.SfietKonstantin.phonebot", 1, 0, "Trigger");
    qmlRegisterUncreatableType<Condition>("org.SfietKonstantin.phonebot", 1, 0, "ConditionBase", REASON);
    qmlRegisterType<JsCondition>("org.SfietKonstantin.phonebot", 1, 0, "Condition");
    qmlRegisterUncreatableType<Action>("org.SfietKonstantin.phonebot", 1, 0, "ActionBase", REASON);
    qmlRegisterType<JsAction>("org.SfietKonstantin.phonebot", 1, 0, "Action");
    qmlRegisterType<Rule>("org.SfietKonstantin.phonebot", 1, 0, "Rule");
    qmlRegisterUncreatableType<AbstractMapper>("org.SfietKonstantin.phonebot", 1, 0, "Mapper", REASON);
    qmlRegisterType<TimeMapper>("org.SfietKonstantin.phonebot", 1, 0, "TimeMapper");

    // Register static plugin paths
    QObjectList staticPlugins = QPluginLoader::staticInstances();
    for (QObject *plugin : staticPlugins) {
        PhoneBotExtensionPlugin *extensionPlugin = qobject_cast<PhoneBotExtensionPlugin *>(plugin);
        if (extensionPlugin) {
            extensionPlugin->registerTypes();
        }
    }

    // Register dynamic plugin paths
    // TODO
}

bool PhoneBotEngine::addComponent(const QUrl &url)
{
    Q_D(PhoneBotEngine);
    if (d->components.contains(url) || d->componentErrors.contains(url)) {
        return false;
    }
    QQmlComponent *component = new QQmlComponent(this, url, QQmlComponent::Asynchronous, this);
    if(component->isLoading()) {
        connect(component, SIGNAL(statusChanged(QQmlComponent::Status)),
                this, SLOT(slotComponentFinished(QQmlComponent::Status)));
    } else {
        d->loadedComponents.append(component);
        QCoreApplication::instance()->postEvent(this, new QEvent(QEvent::User));
    }
    return true;
}

QQmlComponent * PhoneBotEngine::component(const QUrl &url) const
{
    Q_D(const PhoneBotEngine);
    return d->components.value(url, 0);
}

QString PhoneBotEngine::componentError(const QUrl &url)const
{
    Q_D(const PhoneBotEngine);
    return d->componentErrors.value(url, QString());
}

Rule * PhoneBotEngine::rule(const QUrl &url) const
{
    Q_D(const PhoneBotEngine);
    return d->rules.value(url, 0);
}

QString PhoneBotEngine::ruleError(const QUrl &url)const
{
    Q_D(const PhoneBotEngine);
    return d->ruleErrors.value(url, QString());
}

void PhoneBotEngine::start()
{
    Q_D(PhoneBotEngine);
    d->ruleErrors.clear();
    for (QQmlComponent *component : d->components) {
        QUrl url = component->url();
        QObject *ruleObject = component->create();
        if (!ruleObject) {
            d->setRuleError(url, "Rule cannot be created from component.");
            continue;
        }

        Rule *rule = qobject_cast<Rule *>(ruleObject);
        if (!rule) {
            d->setRuleError(url, "The component did not create a Rule type.");
            continue;
        }

        if (!PhoneBotEnginePrivate::checkRule(qobject_cast<Rule *>(rule))) {
            ruleObject->deleteLater();
            d->setRuleError(url, "Invalid Rule created. Check if trigger and actions are set.");
            continue;
        }

        if (d->rules.contains(url)) {
            PhoneBotEnginePrivate::deleteRule(d->rules.value(url));
        }
        d->rules.insert(component->url(), rule);
    }
}

void PhoneBotEngine::stop()
{
    Q_D(PhoneBotEngine);
    d->ruleErrors.clear();

    for (Rule *rule : d->rules) {
        PhoneBotEnginePrivate::deleteRule(rule);
    }
    d->rules.clear();
}

bool PhoneBotEngine::event(QEvent *e)
{
    Q_D(PhoneBotEngine);
    if (e->type() == QEvent::User) {
        e->accept();
        for (QQmlComponent *component : d->loadedComponents) {
            d->manageComponentFinished(component);
        }
        d->loadedComponents.clear();
        return true;
    }

    return QObject::event(e);
}

#include "moc_phonebotengine.cpp"
