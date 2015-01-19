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

#ifdef QT_QML_DEBUG
#include <QtQuick>
#endif

#ifndef DESKTOP
#include <sailfishapp.h>
#endif
#include <QtGui/QGuiApplication>
#include <QtQuick/QQuickView>
#include <QtCore/QtPlugin>
#include <QtQml/qqml.h>
#include <rulesmodel.h>
#include <ruledefinition.h>
#include <rulecomponentsmodel.h>
#include <phonebothelper.h>
#include <phonebotengine.h>
#include <abstractmetadata.h>
#include <enginemanager.h>

Q_IMPORT_PLUGIN(PhoneBotDebugPlugin)
Q_IMPORT_PLUGIN(PhoneBotProfilePlugin)
Q_IMPORT_PLUGIN(PhoneBotTimePlugin)
Q_IMPORT_PLUGIN(PhoneBotConnmanPlugin)
Q_IMPORT_PLUGIN(PhoneBotAmbiencePlugin)
Q_IMPORT_PLUGIN(PhoneBotNotificationPlugin)

static const char *REASON = "Cannot be created";

static QObject *phonebothelper_singletontype_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    PhoneBotHelper *helper = new PhoneBotHelper();
    return helper;
}

int main(int argc, char *argv[])
{

    // App
#ifndef DESKTOP
    QGuiApplication *app = SailfishApp::application(argc, argv);
#else
    QGuiApplication *app = new QGuiApplication(argc, argv);
    app->setOrganizationName("harbour-phonebot");
    app->setApplicationName("harbour-phonebot");
#endif

    // Register types
    PhoneBotEngine::registerTypes();
    qmlRegisterType<RulesModel>("harbour.phonebot", 1, 0, "RulesModel");
    qmlRegisterType<RuleComponentsModel>("harbour.phonebot", 1, 0, "RuleComponentsModel");
    qmlRegisterUncreatableType<RuleDefinition>("harbour.phonebot", 1, 0, "RuleDefinition", REASON);
    qmlRegisterUncreatableType<RuleDefinitionActionModel>("harbour.phonebot", 1, 0,
                                                          "RuleDefinitionActionModel", REASON);
    qmlRegisterUncreatableType<RuleComponentModel>("harbour.phonebot", 1, 0, "RuleComponentModel",
                                                   REASON);
    qmlRegisterUncreatableType<MetaProperty>("harbour.phonebot", 1, 0, "MetaProperty", REASON);
    qmlRegisterUncreatableType<ChoiceModel>("harbour.phonebot", 1, 0, "ChoiceModel", REASON);
    qmlRegisterSingletonType<PhoneBotHelper>("harbour.phonebot", 1, 0, "PhoneBot",
                                             phonebothelper_singletontype_provider);
    EngineManager *manager = new EngineManager();
    manager->reloadEngine();

    // View
#ifndef DESKTOP
    QQuickView *view = SailfishApp::createView();
    view->setSource(SailfishApp::pathTo("qml/main.qml"));
#else
    QQuickView *view = new QQuickView();
    view->setSource(QUrl("qrc:/qml/main.qml"));
#endif
    view->show();
    int result = app->exec();
    manager->deleteLater();
    delete view;
    delete app;
    return result;
}


