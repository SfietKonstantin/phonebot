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

class TestCondition: public Condition
{
    Q_OBJECT
public:
    explicit TestCondition(QObject *parent = 0);
};

TestCondition::TestCondition(QObject *parent)
    : Condition(parent)
{
}

class TestCondition2: public Condition
{
    Q_OBJECT
    PHONEBOT_METADATA(MetaTestCondition2)
public:
    explicit TestCondition2(QObject *parent = 0);
};

TestCondition2::TestCondition2(QObject *parent)
    : Condition(parent)
{
}

class TestCondition3: public Condition
{
    Q_OBJECT
    PHONEBOT_METADATA(TestCondition3) // Error: the metadata refers to itself
public:
    explicit TestCondition3(QObject *parent = 0);
};

TestCondition3::TestCondition3(QObject *parent)
    : Condition(parent)
{
}

class MetaTestCondition4: public AbstractMetaData
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit MetaTestCondition4(QObject *parent = 0);
protected:
    MetaProperty * getProperty(const QString &property, QObject *parent = 0) const;
};

Q_DECLARE_METATYPE(MetaTestCondition4 *)

MetaTestCondition4::MetaTestCondition4(QObject *parent)
    : AbstractMetaData(parent)
{
}

MetaProperty * MetaTestCondition4::getProperty(const QString &property, QObject *parent) const
{
    if (property == "test") {
        return MetaProperty::createBool(property, parent);
    }

    return 0;
}


class TestCondition4: public Condition
{
    Q_OBJECT
    PHONEBOT_METADATA(MetaTestCondition4)
    Q_PROPERTY(bool test READ test WRITE setTest NOTIFY testChanged)
public:
    explicit TestCondition4(QObject *parent = 0);
    bool test() const;
    void setTest(bool test);
signals:
    void testChanged();
};

TestCondition4::TestCondition4(QObject *parent)
    : Condition(parent)
{
}

bool TestCondition4::test() const
{
    return false;
}

void TestCondition4::setTest(bool test)
{
    Q_UNUSED(test)
}

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
}

void TstMeta::testMeta()
{
    MetaComponent *testConditionMeta = MetaComponent::create("MyTestCondition");
    QVERIFY(!testConditionMeta);

    MetaComponent *testCondition2Meta = MetaComponent::create("MyTestCondition2");
    QVERIFY(!testCondition2Meta);

    MetaComponent *testCondition3Meta = MetaComponent::create("MyTestCondition3");
    QVERIFY(!testCondition3Meta);

    MetaComponent *testCondition4Meta = MetaComponent::create("MyTestCondition4");
    QVERIFY(testCondition4Meta);
    QCOMPARE(testCondition4Meta->properties().count(), 1);
    QCOMPARE(testCondition4Meta->properties().first(), QString("test"));
    MetaProperty *property = testCondition4Meta->property("test");
    QVERIFY(property);
    QCOMPARE(property->type(), QMetaType::Bool);
}


void TstMeta::cleanupTestCase()
{
}


QTEST_MAIN(TstMeta)

#include "tst_meta.moc"
