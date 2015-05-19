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

#include <QtTest/QtTest>
#include <QtTest/QSignalSpy>
#include <QtQml/QQmlComponent>
#include <jsaction.h>
#include <jscondition.h>
#include <phonebotengine.h>
#include <rule.h>
#include <trigger.h>

class SimpleTrigger: public Trigger
{
    Q_OBJECT
public:
    explicit SimpleTrigger(QObject *parent = 0) : Trigger(parent) {}
    void sendSignal() {
        emit triggered();
    }
};

class SimpleCondition: public Condition
{
    Q_OBJECT
public:
    explicit SimpleCondition(QObject *parent = 0) : Condition(parent), m_valid(true) {}
    void setValid(bool valid) {
        m_valid = valid;
    }
    bool isValid(Rule *rule) override
    {
        Q_UNUSED(rule)
        return m_valid;
    }
private:
    bool m_valid;
};

class SimpleAction: public Action
{
    Q_OBJECT
public:
    explicit SimpleAction(QObject *parent = 0) : Action(parent) {}
    bool execute(Rule *rule) override
    {
        Q_UNUSED(rule)
        emit executed();
        return true;
    }
signals:
    void executed();
};

class TstRule : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void testJs();
    void disable();
    void cleanupTestCase();
};

void TstRule::initTestCase()
{
    qmlRegisterType<SimpleTrigger>("org.SfietKonstantin.phonebot.tst_rule", 1, 0, "SimpleTrigger");
    qmlRegisterType<SimpleCondition>("org.SfietKonstantin.phonebot.tst_rule", 1, 0, "SimpleCondition");
    qmlRegisterType<SimpleAction>("org.SfietKonstantin.phonebot.tst_rule", 1, 0, "SimpleAction");
}

void TstRule::testJs()
{
    PhoneBotEngine engine;
    engine.registerTypes();
    qmlRegisterType<SimpleTrigger>("org.SfietKonstantin.phonebot.tst_action", 1, 0, "SimpleTrigger");

    // Insert component
    QUrl source ("qrc:/simpleactionrule.qml");
    engine.addComponent(source);

    // Wait
    QSignalSpy spy(&engine, SIGNAL(componentLoadingFinished(QUrl,bool)));
    while (spy.count() < 1) {
        QTest::qWait(100);
    }

    // Some checks
    engine.start();
    Rule *rule = engine.rule(source);
    rule->setEnabled(true);
    QVERIFY(rule != nullptr);

    Trigger *trigger = rule->trigger();
    QVERIFY(trigger != nullptr);

    SimpleTrigger *simpleTrigger = qobject_cast<SimpleTrigger *>(trigger);
    QVERIFY(simpleTrigger != nullptr);

    Condition *conditionObject = rule->condition();
    QVERIFY(conditionObject != nullptr);

    JsCondition *condition = qobject_cast<JsCondition *>(conditionObject);
    QVERIFY(condition != nullptr);
    QVERIFY(condition->condition().isCallable());

    QQmlListReference actions (rule, "actions");
    QCOMPARE(actions.count(), 1);

    QObject *actionObject = actions.at(0);
    QVERIFY(actionObject != nullptr);

    JsAction *action = qobject_cast<JsAction *>(actionObject);
    QVERIFY(action != nullptr);
    QVERIFY(action->action().isCallable());

    const QMetaObject *conditionMeta = condition->metaObject();
    int propertyIndex = conditionMeta->indexOfProperty("isCurrentlyValid");
    QVERIFY(propertyIndex != -1);
    QMetaProperty property = conditionMeta->property(propertyIndex);

    const QMetaObject *actionMeta = action->metaObject();
    int signalIndex = actionMeta->indexOfSignal("executed()");
    QVERIFY(signalIndex != -1);

    QSignalSpy actionSpy (action, SIGNAL(executed()));
    simpleTrigger->sendSignal();
    QCOMPARE(actionSpy.count(), 1);
    QCOMPARE(property.read(condition).toBool(), true);

    simpleTrigger->sendSignal();
    QCOMPARE(actionSpy.count(), 1);
    QCOMPARE(property.read(condition).toBool(), false);
}

void TstRule::disable()
{
    PhoneBotEngine engine;
    engine.registerTypes();

    // Insert component
    QUrl source ("qrc:/simplerule.qml");
    engine.addComponent(source);

    // Wait
    QSignalSpy spy(&engine, SIGNAL(componentLoadingFinished(QUrl,bool)));
    while (spy.count() < 1) {
        QTest::qWait(100);
    }

    // Some checks
    engine.start();
    Rule *rule = engine.rule(source);
    rule->setEnabled(true);
    QVERIFY(rule != nullptr);

    Trigger *trigger = rule->trigger();
    QVERIFY(trigger != nullptr);

    SimpleTrigger *simpleTrigger = qobject_cast<SimpleTrigger *>(trigger);
    QVERIFY(simpleTrigger != nullptr);

    Condition *condition = rule->condition();
    QVERIFY(condition != nullptr);

    SimpleCondition *simpleCondition = qobject_cast<SimpleCondition *>(condition);
    simpleCondition->setValid(true);
    QVERIFY(simpleCondition != nullptr);

    QQmlListReference actions (rule, "actions");
    QCOMPARE(actions.count(), 2);

    QObject *actionObject1 = actions.at(0);
    QVERIFY(actionObject1 != nullptr);

    SimpleAction *simpleAction1 = qobject_cast<SimpleAction *>(actionObject1);
    QVERIFY(simpleAction1 != nullptr);

    QObject *actionObject2 = actions.at(1);
    QVERIFY(actionObject2 != nullptr);

    SimpleAction *simpleAction2 = qobject_cast<SimpleAction *>(actionObject2);
    QVERIFY(simpleAction2 != nullptr);

    // Standard condition
    QSignalSpy actionSpy1 (simpleAction1, SIGNAL(executed()));
    QSignalSpy actionSpy2 (simpleAction2, SIGNAL(executed()));
    simpleTrigger->sendSignal();
    QCOMPARE(actionSpy1.count(), 1);
    QCOMPARE(actionSpy2.count(), 1);

    // Condition is not true
    simpleCondition->setValid(false);
    simpleTrigger->sendSignal();
    QCOMPARE(actionSpy1.count(), 1);
    QCOMPARE(actionSpy2.count(), 1);

    // Disable condition (should work even if the condition is not true)
    simpleCondition->setEnabled(false);
    simpleTrigger->sendSignal();
    QCOMPARE(actionSpy1.count(), 2);
    QCOMPARE(actionSpy2.count(), 2);

    // Disable an action
    simpleCondition->setEnabled(true);
    simpleCondition->setValid(true);
    simpleAction1->setEnabled(false);
    simpleTrigger->sendSignal();
    QCOMPARE(actionSpy1.count(), 2);
    QCOMPARE(actionSpy2.count(), 3);

    // Disable the rule
    simpleAction1->setEnabled(true);
    rule->setEnabled(false);
    simpleTrigger->sendSignal();
    QCOMPARE(actionSpy1.count(), 2);
    QCOMPARE(actionSpy2.count(), 3);
}

void TstRule::cleanupTestCase()
{
    QTest::qWait(100); // Process delete later
}


QTEST_MAIN(TstRule)

#include "tst_rule.moc"
