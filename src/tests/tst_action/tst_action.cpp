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

#include <QtCore/QMetaObject>
#include <QtTest/QtTest>
#include <QtQml/qqml.h>
#include <jsaction.h>
#include <phonebotengine.h>
#include <rule.h>
#include <trigger.h>

class SimpleTrigger: public Trigger
{
    Q_OBJECT
public:
    explicit SimpleTrigger(QObject *parent = 0);
    void sendSignal();
};

SimpleTrigger::SimpleTrigger(QObject *parent)
    : Trigger(parent)
{
}

void SimpleTrigger::sendSignal()
{
    emit triggered();
}

class TstAction : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void testJsAction();
    void cleanupTestCase();
};

void TstAction::initTestCase()
{
}

void TstAction::testJsAction()
{
    PhoneBotEngine *engine = new PhoneBotEngine(this);
    engine->registerTypes();
    qmlRegisterType<SimpleTrigger>("org.SfietKonstantin.phonebot.tst_action", 1, 0, "SimpleTrigger");

    // Insert component
    QUrl source ("qrc:/simpleactionrule.qml");
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

    Trigger *trigger = rule->trigger();
    QVERIFY(trigger);

    SimpleTrigger *simpleTrigger = qobject_cast<SimpleTrigger *>(trigger);
    QVERIFY(simpleTrigger);

    QQmlListReference actions (rule, "actions");
    QCOMPARE(actions.count(), 1);

    QObject *actionObject = actions.at(0);
    QVERIFY(actionObject);

    JsAction *action = qobject_cast<JsAction *>(actionObject);
    QVERIFY(action);

    const QMetaObject *meta = action->metaObject();
    int signalIndex = meta->indexOfSignal("executed()");
    QVERIFY(signalIndex != -1);

    QSignalSpy actionSpy (action, SIGNAL(executed()));
    simpleTrigger->sendSignal();
    QCOMPARE(actionSpy.count(), 1);
}

void TstAction::cleanupTestCase()
{
}


QTEST_MAIN(TstAction)

#include "tst_action.moc"
