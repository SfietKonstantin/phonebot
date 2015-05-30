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
    void testParseRule();
    void testParseMemoryLeakRule();
    void testDocument();
    void testParseError();
    void cleanupTestCase();
};

void TstParser::initTestCase()
{
}

class DummyShared
{
public:
    typedef QSharedPointer<DummyShared> Ptr;
    DummyShared(const DummyShared &) = delete;
    DummyShared(DummyShared &&) = delete;
    DummyShared & operator=(const DummyShared &) = delete;
    DummyShared & operator=(DummyShared &&) = delete;
    ~DummyShared()
    {
        m_destructorCalled = true;
    }
    static Ptr create(bool &destructorCalled)
    {
        destructorCalled = false;
        return Ptr(new DummyShared(destructorCalled));
    }
private:
    explicit DummyShared(bool &destructorCalled) : m_destructorCalled(destructorCalled) {}
    bool &m_destructorCalled;
};

Q_DECLARE_METATYPE(DummyShared::Ptr)

void TstParser::testParseRule()
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
    QCOMPARE(root->properties().count(), 9);
    QCOMPARE(root->type(), QString("Rectangle"));
    QCOMPARE(root->id(), QString("test"));

    // Properties
    QVERIFY(root->hasProperty("width"));
    QCOMPARE(root->property("width"), QVariant(100));
    QVERIFY(root->hasProperty("text"));
    QCOMPARE(root->property("text"), QVariant("Test"));
    QVERIFY(root->hasProperty("enabled"));
    QCOMPARE(root->property("enabled"), QVariant(true));
    QVERIFY(root->hasProperty("visible"));
    QCOMPARE(root->property("visible"), QVariant(false));
    QVERIFY(root->hasProperty("item"));
    QmlObject::Ptr item = root->property("item").value<QmlObject::Ptr>();
    QVERIFY(!item.isNull());
    QCOMPARE(item->type(), QString("Item"));
    QCOMPARE(item->id(), QString("subTest"));
    QVERIFY(root->hasProperty("anchors.top"));
    QVariant anchorsTopProperty = root->property("anchors.top");
    QVERIFY(anchorsTopProperty.canConvert<Reference::Ptr>());
    QCOMPARE(anchorsTopProperty.value<Reference::Ptr>()->value(), QString("parent.top"));
    QVERIFY(root->hasProperty("js"));
    QVariant jsProperty = root->property("js");
    QVERIFY(jsProperty.canConvert<Expression::Ptr>());
    QCOMPARE(jsProperty.value<Expression::Ptr>()->value(), QString("new Date(1960, 1, 1)"));
    QVERIFY(root->hasProperty("array"));
    QVariant arrayProperty = root->property("array");
    QVERIFY(arrayProperty.canConvert<Expression::Ptr>());
    QCOMPARE(arrayProperty.value<Expression::Ptr>()->value(), QString("[1, 2, 3]"));
    QVariant childrenProperty = root->property("children");
    QVERIFY(childrenProperty.canConvert<QVariantList>());
    const QVariantList &childrenList = childrenProperty.toList();
    QCOMPARE(childrenList.count(), 1);
    QVERIFY(childrenList.first().canConvert<QmlObject::Ptr>());
    QCOMPARE(childrenList.first().value<QmlObject::Ptr>()->type(), QString("Item"));

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
    QVERIFY(testAnchorsLeft.canConvert<Reference::Ptr>());
    QCOMPARE(testAnchorsLeft.value<Reference::Ptr>()->value(), QString("test.anchors.right"));
}

void TstParser::testParseMemoryLeakRule()
{
    // Used to debug the tricky QmlObject::~QmlObject not called issue
    // Basically, QmlObject contains a hiearchy of QmlObjects, and QSharedPointer
    // creates a destruction hiearchy. However, during parsing, this hiearchy is not
    // ready, making some objects not destroyable.
    QWeakPointer<QmlObject> weakRoot;
    {
        QString fileName = ":/memoryleaktest.qml";
        QmlDocument::Ptr document = QmlDocument::create(fileName);
        QVERIFY(!document.isNull());
        QVERIFY(!document->rootObject().isNull());
        weakRoot = document->rootObject().toWeakRef();
    }
    QVERIFY(weakRoot.isNull());
}

void TstParser::testDocument()
{
    QmlObject::Ptr object = QmlObject::create("Test");
    object->setId("test");

    QmlObject::Ptr small = QmlObject::create("Small");
    QList<QmlObject::Ptr>  children;
    children.append(small);
    object->setChildren(children);

    QVariantMap properties;
    QVariantList intList;
    properties.insert("boolTrue", QVariant(true));
    properties.insert("boolFalse", QVariant(false));
    properties.insert("float", QVariant(123.45));
    intList << 1 << 2 << 3 << 4;
    properties.insert("intList", intList);
    QVariantList objectList;
    objectList << QVariant::fromValue(small);
    properties.insert("objectList", objectList);
    QStringList fieldValues;
    fieldValues << "root" << "reference";
    properties.insert("reference", QVariant::fromValue(Reference::create("parent", fieldValues)));
    properties.insert("string", "My test string");
    object->setProperties(properties);
    QString result;
    QTextStream resultStream(&result);
    resultStream << "Test {" << endl
                 << "    id: test" << endl
                 << "    boolFalse: false" << endl
                 << "    boolTrue: true" << endl
                 << "    float: 123.45" << endl
                 << "    intList: [" << endl
                 << "        1," << endl
                 << "        2," << endl
                 << "        3," << endl
                 << "        4" << endl
                 << "    ]" << endl
                 << "    objectList: [" << endl
                 << "        Small {" << endl
                 << "        }" << endl
                 << "    ]" << endl
                 << "    reference: parent.root.reference" << endl
                 << "    string: \"My test string\"" << endl
                 << "    Small {" << endl
                 << "    }" << endl
                 << "}" << endl;
    QCOMPARE(object->toString(0), result);

    WritableQmlDocument::Ptr document = WritableQmlDocument::create();
    document->addImport(ImportStatement::createImport("QtQuick", "2.0", "QQ"));
    document->addImport(ImportStatement::createFileImport("Test.js"));
    document->setRootObject(object);

    QString documentResult;
    QTextStream documentResultStream(&documentResult);
    documentResultStream << "import QtQuick 2.0 as QQ" << endl
                         << "import \"Test.js\"" << endl
                         << endl
                         << result;
    QCOMPARE(document->toString(), documentResult);
}

void TstParser::testParseError()
{
    // Invalid file
    QmlDocument::Ptr invalidDocument = QmlDocument::create(":/invalid.qml");
    QVERIFY(invalidDocument.isNull());

    // Parse error
    QmlDocument::Ptr parseErrorDocument = QmlDocument::create(":/parseerrorrule.qml");
    QVERIFY(!parseErrorDocument.isNull());
    QCOMPARE(parseErrorDocument->error(), QmlDocument::ParseError);
    QVERIFY(!parseErrorDocument->errorMessage().isEmpty());
}

void TstParser::cleanupTestCase()
{
}


QTEST_MAIN(TstParser)

#include "tst_parser.moc"
