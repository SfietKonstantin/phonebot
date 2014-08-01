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
#include <qmldocument.h>

class TstParser : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void parseRule();
    void testDocument();
    void cleanupTestCase();
};

void TstParser::initTestCase()
{
}

void TstParser::parseRule()
{
    QString fileName = ":/simpletest.qml";
    QmlDocument::Ptr document = QmlDocument::create(fileName);
    QVERIFY(!document.isNull());
    QCOMPARE(document->fileName(), fileName);

    // Verify imports
    QCOMPARE(document->imports().count(), 4);
    ImportStatement::Ptr import = document->imports()[0];
    QCOMPARE(import->importUri(), QString("QtQuick"));
    QVERIFY(import->importFile().isEmpty());
    QCOMPARE(import->version(), QString("2.0"));
    QVERIFY(import->importId().isEmpty());
    import = document->imports()[1];
    QVERIFY(import->importUri().isEmpty());
    QCOMPARE(import->importFile(), QString("Test.qml"));
    QCOMPARE(import->version(), QString("1.0"));
    QCOMPARE(import->importId(), QString("Test"));
    import = document->imports()[2];
    QCOMPARE(import->importUri(), QString("QtQuick"));
    QVERIFY(import->importFile().isEmpty());
    QCOMPARE(import->version(), QString("2.0"));
    QCOMPARE(import->importId(), QString("QQ"));
    import = document->imports()[3];
    QVERIFY(import->importUri().isEmpty());
    QCOMPARE(import->importFile(), QString("Test.qml"));
    QVERIFY(import->version().isEmpty());
    QVERIFY(import->importId().isEmpty());




    QmlObject::Ptr root = document->rootObject();
    QVERIFY(!root.isNull());
    QCOMPARE(root->properties().count(), 6);
    QCOMPARE(root->type(), QString("Rectangle"));
    QCOMPARE(root->id(), QString("test"));

    // Properties
    QVERIFY(root->hasProperty("width"));
    QCOMPARE(root->property("width"), QVariant(100));
    QVERIFY(root->hasProperty("text"));
    QCOMPARE(root->property("text"), QVariant("Test"));
    QVERIFY(root->hasProperty("item"));
    QmlObject::Ptr item = root->property("item").value<QmlObject::Ptr>();
    QVERIFY(!item.isNull());
    QCOMPARE(item->type(), QString("Item"));
    QCOMPARE(item->id(), QString("subTest"));
    QVERIFY(root->hasProperty("anchors.top"));
    QVariant anchorsTopProperty = root->property("anchors.top");
    QVERIFY(anchorsTopProperty.canConvert<Reference>());
    QCOMPARE(anchorsTopProperty.value<Reference>().value(), QString("parent.top"));
    QVERIFY(root->hasProperty("js"));
    QVariant jsProperty = root->property("js");
    QVERIFY(jsProperty.canConvert<Expression>());
    QCOMPARE(jsProperty.value<Expression>().value(), QString("new Date(1960, 1, 1)"));
    QVERIFY(root->hasProperty("array"));

    // Children
    QCOMPARE(root->children().count(), 2);
    QmlObject::Ptr child = root->children().first();
    QVERIFY(!child.isNull());
    QCOMPARE(child->type(), QString("Text"));
    QCOMPARE(child->properties().count(), 1);
    QVERIFY(child->hasProperty("text"));
    QCOMPARE(child->property("text").toString(), QString("Test 2"));

    child = root->children()[1];
    QCOMPARE(child->type(), QString("Test"));
    QCOMPARE(child->properties().count(), 1);
    QVERIFY(child->hasProperty("test.anchors.left"));
    QVariant testAnchorsLeft = child->property("test.anchors.left");
    QVERIFY(testAnchorsLeft.canConvert<Reference>());
    QCOMPARE(testAnchorsLeft.value<Reference>().value(), QString("test.anchors.right"));
}

void TstParser::testDocument()
{
    QmlObject::Ptr root = QmlObject::create("Test");
    root->setId("test");

    QVariantMap properties;
    QVariantList intList;
    intList << 1 << 2 << 3 << 4;
    properties.insert("intList", intList);
    QString string = "My test string";
    properties.insert("string", string);
    root->setProperties(properties);
    qWarning() << root->toString(0);

    QmlObject::Ptr superRoot = QmlObject::create("Super");
    QVariantMap superProperties;
    QVariantList objectList;
    objectList << QVariant::fromValue(root);
    superProperties.insert("objectList", objectList);
    superRoot->setProperties(superProperties);
    qWarning() << superRoot->toString(0);
}

void TstParser::cleanupTestCase()
{
}


QTEST_MAIN(TstParser)

#include "tst_parser.moc"
