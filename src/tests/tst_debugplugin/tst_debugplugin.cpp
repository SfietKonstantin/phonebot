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

#include <QtCore/QtPlugin>
#include <QtTest/QtTest>
#include <QtDBus/QDBusInterface>
#include <QtQml/qqml.h>
#include <action.h>
#include <phonebotengine.h>
#include <rule.h>

Q_IMPORT_PLUGIN(PhoneBotDebugPlugin)

class PongAction: public Action
{
    Q_OBJECT
public:
    explicit PongAction(QObject *parent = 0) : Action(parent) {}
    bool execute(Rule *rule, QString &error) override
    {
        Q_UNUSED(rule);
        Q_UNUSED(error);
        emit pong();
        return true;
    }
Q_SIGNALS:
    void pong();
};

class TstDebugPlugin : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void testDebugTrigger();
    void cleanupTestCase();
};

void TstDebugPlugin::initTestCase()
{
}

void TstDebugPlugin::testDebugTrigger()
{
    PhoneBotEngine *engine = new PhoneBotEngine(this);
    engine->registerTypes();
    qmlRegisterType<PongAction>("org.SfietKonstantin.phonebot.tst_debugplugin", 1, 0, "PongAction");

    // Insert component
    QUrl source ("qrc:/debugrule.qml");
    engine->addComponent(source);

    // Wait
    QSignalSpy spy(engine, SIGNAL(componentLoadingFinished(QUrl,bool)));
    while (spy.count() < 1) {
        QTest::qWait(100);
    }

    // Some checks
    engine->start();
    Rule *rule = engine->rule(source);
    QVERIFY(rule);

    QQmlListReference actions (rule, "actions");
    QCOMPARE(actions.count(), 2);

    QObject *actionObject = actions.at(0);
    QVERIFY(actionObject);

    PongAction *action = qobject_cast<PongAction *>(actionObject);
    QVERIFY(action);

    QSignalSpy actionSpy(action, SIGNAL(pong()));
    QTest::qWait(100);

    QDBusInterface interface ("org.SfietKonstantin.phonebotdebug", "/test",
                              "org.SfietKonstantin.phonebotdebug");
    QVERIFY(interface.isValid());
    QDBusMessage message = interface.call("Ping");
    QCOMPARE(message.type(), QDBusMessage::ReplyMessage);
    QCOMPARE(actionSpy.count(), 1);
    engine->stop();
}

void TstDebugPlugin::cleanupTestCase()
{
    QTest::qWait(100); // Process delete later
}


QTEST_MAIN(TstDebugPlugin)

#include "tst_debugplugin.moc"
