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
#include <QtQml/qqml.h>
#include <abstractmetadata.h>
#include <condition.h>
#include <metacomponent.h>
#include <phonebotmeta.h>
#include <phonebotengine.h>
#include <metatypecache.h>

class TestCondition: public Condition
{
    Q_OBJECT
public:
    explicit TestCondition(QObject *parent = 0) : Condition(parent) { }
    bool isValid(Rule *rule) override
    {
        Q_UNUSED(rule);
        return false;
    }
};

class TestCondition2: public Condition
{
    Q_OBJECT
    PHONEBOT_METADATA(MetaTestCondition2)
public:
    explicit TestCondition2(QObject *parent = 0) : Condition(parent) {}
    bool isValid(Rule *rule) override
    {
        Q_UNUSED(rule);
        return false;
    }
};

class TestCondition3: public Condition
{
    Q_OBJECT
    PHONEBOT_METADATA(TestCondition3) // Error: the metadata refers to itself
public:
    explicit TestCondition3(QObject *parent = 0) : Condition(parent) {}
    bool isValid(Rule *rule) override
    {
        Q_UNUSED(rule);
        return false;
    }
};

class MetaTestCondition4: public AbstractMetaData
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit MetaTestCondition4(QObject *parent = 0) : AbstractMetaData(parent) {}
    QString name() const override
    {
        return "No name";
    }
    QString description() const override
    {
        return "No description";
    }
    QString summary(const QVariantMap &properties) const override
    {
        Q_UNUSED(properties);
        return "No summary";
    }
protected:
    MetaProperty * getProperty(const QString &property, QObject *parent = 0) const override
    {
        if (property == "test") {
            return MetaProperty::createBool(property, "test", parent);
        }
        return nullptr;
    }
};

Q_DECLARE_METATYPE(MetaTestCondition4 *)

class TestCondition4: public Condition
{
    Q_OBJECT
    PHONEBOT_METADATA(MetaTestCondition4)
    Q_PROPERTY(bool test READ test WRITE setTest NOTIFY testChanged)
public:
    explicit TestCondition4(QObject *parent = 0) : Condition(parent) {}
    bool test() const
    {
        return false;
    }
    void setTest(bool test)
    {
        Q_UNUSED(test)
    }
    bool isValid(Rule *rule) override
    {
        Q_UNUSED(rule);
        return false;
    }
signals:
    void testChanged();
};

class TstMeta : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void testMeta();
   void cleanupTestCase();
};

void TstMeta::initTestCase()
{
    qmlRegisterType<TestCondition>("org.SfietKonstantin.phonebot.tst_meta", 1, 0, "MyTestCondition");
    qmlRegisterType<TestCondition2>("org.SfietKonstantin.phonebot.tst_meta", 1, 0, "MyTestCondition2");
    qmlRegisterType<TestCondition3>("org.SfietKonstantin.phonebot.tst_meta", 1, 0, "MyTestCondition3");
    qmlRegisterType<TestCondition4>("org.SfietKonstantin.phonebot.tst_meta", 1, 0, "MyTestCondition4");
    qRegisterMetaType<MetaTestCondition4 *>();
    PhoneBotEngine::registerTypes();
}

void TstMeta::testMeta()
{
    MetaTypeCache cache;

    QVERIFY(!cache.metaData("MyTestCondition"));
    QVERIFY(!cache.metaData("MyTestCondition2"));
    QVERIFY(!cache.metaData("MyTestCondition3"));

    AbstractMetaData *testCondition4Meta = cache.metaData("MyTestCondition4");
    QStringList testCondition4Properties = cache.properties("MyTestCondition4");
    QVERIFY(testCondition4Meta);
    QCOMPARE(testCondition4Properties.count(), 1);
    QCOMPARE(testCondition4Properties.first(), QString("test"));
    MetaProperty *property = testCondition4Meta->property("test");
    QVERIFY(property);
    QCOMPARE(property->type(), MetaProperty::Bool);
}

void TstMeta::cleanupTestCase()
{
    QTest::qWait(100); // Process delete later
}

QTEST_MAIN(TstMeta)

#include "tst_meta.moc"
