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
#include <action.h>
#include <condition.h>
#include <phonebotengine.h>
#include <trigger.h>

class DummyTrigger: public Trigger
{
    Q_OBJECT
public:
    explicit DummyTrigger(QObject *parent = 0);
};

DummyTrigger::DummyTrigger(QObject *parent)
    : Trigger(parent)
{
}

class DummyCondition: public Condition
{
    Q_OBJECT
public:
    explicit DummyCondition(QObject *parent = 0);
    bool isValid(Rule *rule);
};

DummyCondition::DummyCondition(QObject *parent)
    : Condition(parent)
{
}

bool DummyCondition::isValid(Rule *rule)
{
    Q_UNUSED(rule)
    return false;
}

class DummyAction: public Action
{
    Q_OBJECT
public:
    explicit DummyAction(QObject *parent = 0);
    bool execute(Rule *rule);
};

DummyAction::DummyAction(QObject *parent)
    : Action(parent)
{
}

bool DummyAction::execute(Rule *rule)
{
    Q_UNUSED(rule)
    return false;
}

class TstPhoneBotEngine : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void components();
    void rules();
    void cleanupTestCase();
};

void TstPhoneBotEngine::initTestCase()
{
    qmlRegisterType<DummyTrigger>("org.SfietKonstantin.phonebot.tst_phonebotengine", 1, 0, "DummyTrigger");
    qmlRegisterType<DummyCondition>("org.SfietKonstantin.phonebot.tst_phonebotengine", 1, 0, "DummyCondition");
    qmlRegisterType<DummyAction>("org.SfietKonstantin.phonebot.tst_phonebotengine", 1, 0, "DummyAction");
}

void TstPhoneBotEngine::components()
{
    PhoneBotEngine *engine = new PhoneBotEngine(this);
    engine->registerTypes();

    // Insert a simple component
    QUrl source ("qrc:/dummy.qml");
    engine->addComponent(source);

    // Check that component is not available
    QVERIFY(!engine->component(source));

    QSignalSpy spy(engine, SIGNAL(componentLoadingFinished(QUrl,bool)));
    while (spy.count() == 0) {
        QTest::qWait(100);
    }
    // Check signal return
    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toUrl(), source);
    QCOMPARE(arguments.at(1).toBool(), true);

    // Check if component is available
    QVERIFY(engine->component(source));
    QCOMPARE(engine->component(source)->url(), source);
    QVERIFY(engine->componentError(source).isNull());

    // Insert a simple component with error
    QUrl sourceFailure ("qrc:/dummy_error.qml");

    engine->addComponent(sourceFailure);

    // Check that component is not available
    QVERIFY(!engine->component(sourceFailure));

    QSignalSpy spyFailure(engine, SIGNAL(componentLoadingFinished(QUrl,bool)));
    while (spyFailure.count() == 0) {
        QTest::qWait(100);
    }
    // Check signal return
    QCOMPARE(spyFailure.count(), 1);
    arguments = spyFailure.takeFirst();
    QCOMPARE(arguments.at(0).toUrl(), sourceFailure);
    QCOMPARE(arguments.at(1).toBool(), false);

    // Check if component is not available
    QVERIFY(!engine->component(sourceFailure));
    QVERIFY(!engine->componentError(sourceFailure).isEmpty());

    // Insert a more complex component
    QUrl complexSource ("qrc:/simplerule.qml");
    engine->addComponent(complexSource);

    // Check that component is not available
    QVERIFY(!engine->component(complexSource));

    QSignalSpy complexSpy(engine, SIGNAL(componentLoadingFinished(QUrl,bool)));
    while (complexSpy.count() == 0) {
        QTest::qWait(100);
    }
    // Check signal return
    QCOMPARE(complexSpy.count(), 1);
    arguments = complexSpy.takeFirst();
    QCOMPARE(arguments.at(0).toUrl(), complexSource);
    QCOMPARE(arguments.at(1).toBool(), true);

    // Check if component is available
    QVERIFY(engine->component(complexSource));
    QCOMPARE(engine->component(complexSource)->url(), complexSource);
    QVERIFY(engine->componentError(complexSource).isNull());

    // Reinsert
    QVERIFY(!engine->addComponent(complexSource));
}

void TstPhoneBotEngine::rules()
{
    PhoneBotEngine *engine = new PhoneBotEngine(this);
    engine->registerTypes();

    // Insert a simple component
    QUrl sourceNoRule ("qrc:/no_rule.qml");
    QUrl sourceBadRule ("qrc:/dummy.qml");
    QUrl sourceSimpleRule ("qrc:/simplerule.qml");
    engine->addComponent(sourceNoRule);
    engine->addComponent(sourceBadRule);
    engine->addComponent(sourceSimpleRule);

    // Wait
    QSignalSpy spy(engine, SIGNAL(componentLoadingFinished(QUrl,bool)));
    while (spy.count() != 3) {
        QTest::qWait(100);
    }

    // Start
    engine->start();

    QVERIFY(!engine->rule(sourceNoRule));
    QVERIFY(!engine->ruleError(sourceNoRule).isEmpty());
    QVERIFY(!engine->rule(sourceBadRule));
    QVERIFY(!engine->ruleError(sourceBadRule).isEmpty());
    QVERIFY(engine->rule(sourceSimpleRule));
    QVERIFY(engine->ruleError(sourceSimpleRule).isEmpty());

    engine->stop();
    QVERIFY(!engine->rule(sourceNoRule));
    QVERIFY(engine->ruleError(sourceNoRule).isNull());
    QVERIFY(!engine->rule(sourceBadRule));
    QVERIFY(engine->ruleError(sourceBadRule).isNull());
    QVERIFY(!engine->rule(sourceSimpleRule));
    QVERIFY(engine->ruleError(sourceSimpleRule).isNull());

}

void TstPhoneBotEngine::cleanupTestCase()
{
}


QTEST_MAIN(TstPhoneBotEngine)

#include "tst_phonebotengine.moc"
