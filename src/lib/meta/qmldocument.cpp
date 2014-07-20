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

#include "qmldocument.h"
#include <QtCore/QFile>
#ifdef QMLDOCUMENT_DEBUG
#include <QtCore/QDebug>
#endif
#include <QtCore/QStack>
#include "qmljsengine_p.h"
#include "qmljslexer_p.h"
#include "qmljsparser_p.h"

struct ImportStatementPrivate
{
    QString importUri;
    QString importFile;
    QString version;
    QString importId;
};

ImportStatement::ImportStatement()
    : d_ptr(new ImportStatementPrivate)
{
}

ImportStatement::ImportStatement(ImportStatementPrivate &dd)
    : d_ptr(&dd)
{
}

ImportStatement::~ImportStatement()
{
}

QString ImportStatement::importUri() const
{
    Q_D(const ImportStatement);
    return d->importUri;
}

QString ImportStatement::importFile() const
{
    Q_D(const ImportStatement);
    return d->importFile;
}

QString ImportStatement::version() const
{
    Q_D(const ImportStatement);
    return d->version;
}

QString ImportStatement::importId() const
{
    Q_D(const ImportStatement);
    return d->importId;
}

class CreatableImportStatement: public ImportStatement
{
public:
    explicit CreatableImportStatement(const QString &importUri, const QString &importFile,
                                      const QString &version, const QString &importId);
private:
    Q_DECLARE_PRIVATE(ImportStatement)
};

CreatableImportStatement::CreatableImportStatement(const QString &importUri,
                                                   const QString &importFile,
                                                   const QString &version,
                                                   const QString &importId)
    : ImportStatement(*(new ImportStatementPrivate))
{
    Q_D(ImportStatement);
    d->importUri = importUri;
    d->importFile = importFile;
    d->version = version;
    d->importId = importId;
}


Expression::Expression()
{
}

Expression::Expression(const QString &expression)
    : m_expression(expression)
{
}

QString Expression::value() const
{
    return m_expression;
}

// Subclass exposing a creator + d-pointer
class CreatableQmlObject: public QmlObject
{
public:
    typedef QSharedPointer<CreatableQmlObject> Ptr;
    typedef QSharedPointer<const CreatableQmlObject> ConstPtr;
    explicit CreatableQmlObject();
    static CreatableQmlObject::Ptr create();
    static QmlObject::Ptr toObject(Ptr creatable);
    QmlObjectPrivate * d();
    const QmlObjectPrivate * d() const;
private:
    Q_DECLARE_PRIVATE(QmlObject)
};

struct QmlObjectPrivate
{
    CreatableQmlObject::Ptr parent; // Used internally when parsing
    QString type;
    QString id;
    QVariantMap properties;
    QList<QmlObject::Ptr> children;
};

CreatableQmlObject::CreatableQmlObject()
    : QmlObject(*(new QmlObjectPrivate))
{
}

CreatableQmlObject::Ptr CreatableQmlObject::create()
{
    return CreatableQmlObject::Ptr(new CreatableQmlObject());
}

QmlObject::Ptr CreatableQmlObject::toObject(CreatableQmlObject::Ptr creatable)
{
    return qSharedPointerCast<QmlObject>(creatable);
}

QmlObjectPrivate * CreatableQmlObject::d()
{
    Q_D(QmlObject);
    return d;
}

const QmlObjectPrivate * CreatableQmlObject::d() const
{
    Q_D(const QmlObject);
    return d;
}

QmlObject::QmlObject()
    : d_ptr(new QmlObjectPrivate())
{
}

QmlObject::QmlObject(QmlObjectPrivate &dd)
    : d_ptr(&dd)
{
}

QmlObject::~QmlObject()
{
}

QString QmlObject::type() const
{
    Q_D(const QmlObject);
    return d->type;
}

QString QmlObject::id() const
{
    Q_D(const QmlObject);
    return d->id;
}

QStringList QmlObject::properties() const
{
    Q_D(const QmlObject);
    return d->properties.keys();
}

bool QmlObject::hasProperty(const QString &key) const
{
    Q_D(const QmlObject);
    return d->properties.contains(key);
}

QVariant QmlObject::property(const QString &key) const
{
    Q_D(const QmlObject);
    return d->properties.value(key);
}

QList<QmlObject::Ptr> QmlObject::children() const
{
    Q_D(const QmlObject);
    return d->children;
}

// Visitor

// A buffer to receive expressions. Used to store JS expressions,
// or simple literal from bindings
struct ExpressionBuffer
{
    typedef QSharedPointer<ExpressionBuffer> Ptr;
    static Ptr create()
    {
        return Ptr(new ExpressionBuffer);
    }
    QmlObject::Ptr object;
    QString source;
    QVariantList buffer;
private:
    ExpressionBuffer() {}
};

class QmlVisitor: protected QmlJS::AST::Visitor
{
public:
    QmlObject::Ptr operator()(const QString &source, QList<ImportStatement::Ptr> &imports,
                              QmlJS::AST::Node *node)
    {
        _imports = &imports;
        _imports->clear();
        _source = source;
        QmlJS::AST::Node::accept(node, this);
        return _current;
    }
protected:
#ifdef QMLDOCUMENT_DEBUG
    bool preVisit(QmlJS::AST::Node *ast)
    {
        qDebug() << "Begin:" << ast->kind;
        qDebug() << "Full:";
        int offset = ast->firstSourceLocation().offset;
        int length = ast->lastSourceLocation().offset + ast->lastSourceLocation().length - offset;
        qDebug() << _source.mid(offset, length);
        qDebug() << "First";
        length = ast->firstSourceLocation().length;
        qDebug() << _source.mid(offset, length);
        return true;
    }

    void postVisit(QmlJS::AST::Node *ast)
    {
        qDebug() << "End:  " << ast->kind;
    }
#endif

    // Manage the import
    bool visit(QmlJS::AST::UiImport *ast)
    {
        QString importUri;
        if (ast->importUri) {
            importUri = ast->importUri->name.toString();
        }

        int offset = ast->versionToken.offset;
        int length = ast->versionToken.length;
        QString version = _source.mid(offset, length);

        QString fileName = ast->fileName.toString();
        QString importId = ast->importId.toString();

        ImportStatement::Ptr import
                = ImportStatement::Ptr(new CreatableImportStatement(importUri, fileName, version,
                                                                    importId));
        _imports->append(import);
        return true;
    }

    // Manage bindings
    bool visit(QmlJS::AST::UiScriptBinding *ast)
    {
        Q_ASSERT(ast->qualifiedId);
        visitBindings(ast->qualifiedId);
        return true;
    }

    void endVisit(QmlJS::AST::UiScriptBinding *)
    {
        endVisitBindings();
    }

    bool visit(QmlJS::AST::ExpressionStatement *ast)
    {
        Q_ASSERT(!_buffers.isEmpty());
        ExpressionBuffer::Ptr buffer = _buffers.top();

        int offset = ast->firstSourceLocation().offset;
        int length = ast->lastSourceLocation().offset + ast->lastSourceLocation().length - offset;
        buffer->source = _source.mid(offset, length);
        return true;
    }

    void endVisit(QmlJS::AST::ExpressionStatement *)
    {
        Q_ASSERT(!_buffers.isEmpty());
        ExpressionBuffer::Ptr buffer = _buffers.top();

        // If we found an unparsable entry, we just put
        // the source
        if (buffer->buffer.count() != 1) {
            buffer->buffer.clear();
            buffer->buffer.append(QVariant::fromValue(Expression(buffer->source)));
        }
    }

    bool visit(QmlJS::AST::NumericLiteral *ast)
    {
        Q_ASSERT(!_bindings.isEmpty());
        _buffers.top()->buffer.append(ast->value);
        return true;
    }

    bool visit(QmlJS::AST::StringLiteral *ast)
    {
        Q_ASSERT(!_bindings.isEmpty());
        _buffers.top()->buffer.append(ast->value.toString());
        return true;
    }

    bool visit(QmlJS::AST::TrueLiteral *)
    {
        Q_ASSERT(!_bindings.isEmpty());
        _buffers.top()->buffer.append(QVariant(true));
        return true;
    }

    bool visit(QmlJS::AST::FalseLiteral *)
    {
        Q_ASSERT(!_bindings.isEmpty());
        _buffers.top()->buffer.append(QVariant(false));
        return true;
    }

    // Manage object bindings
    bool visit(QmlJS::AST::UiObjectBinding *ast)
    {
        Q_ASSERT(ast->qualifiedId);
        Q_ASSERT(ast->qualifiedTypeNameId);
        visitBindings(ast->qualifiedId);

        // Also push an object
        CreatableQmlObject::Ptr object = CreatableQmlObject::create();
        object->d()->parent = _current;
        object->d()->type = ast->qualifiedTypeNameId->name.toString();;
        _current = object;

        // Directly insert the object inside the buffer
        _buffers.top()->buffer.append(QVariant::fromValue(CreatableQmlObject::toObject(object)));
        return true;
    }

    void endVisit(QmlJS::AST::UiObjectBinding *)
    {
        Q_ASSERT(!_current->d()->parent.isNull());
        _current = _current->d()->parent;
        endVisitBindings();
    }

    // Component management
    bool visit(QmlJS::AST::UiObjectDefinition *ast)
    {
        CreatableQmlObject::Ptr object = CreatableQmlObject::create();
        object->d()->parent = _current;
        object->d()->type = ast->qualifiedTypeNameId->name.toString();

        _current = object;
        return true;
    }

    void endVisit(QmlJS::AST::UiObjectDefinition *)
    {
        if (_current->d()->parent.isNull()) {
            return;
        }

        _current->d()->parent->d()->children.append(_current);
        _current = _current->d()->parent;
    }

private:
    // Helper functions
    void visitBindings(QmlJS::AST::UiQualifiedId *key)
    {
        int offset = key->firstSourceLocation().offset;
        int length = key->lastSourceLocation().offset + key->lastSourceLocation().length - offset;
        _bindings.push(_source.mid(offset, length));

        // Also push a buffer
        _buffers.push(ExpressionBuffer::create());
    }

    void endVisitBindings()
    {
        Q_ASSERT(!_buffers.isEmpty());
        Q_ASSERT(!_bindings.isEmpty());
        Q_ASSERT(!_current.isNull());
        QString binding = _bindings.pop();
        ExpressionBuffer::Ptr buffer = _buffers.pop();
        Q_ASSERT(buffer->buffer.count() == 1);

        QVariant value = buffer->buffer.first();
        if (binding == "id" && value.canConvert<Expression>()) {
            Expression id = value.value<Expression>();
            _current->d()->id = id.value();
        } else {
            _current->d()->properties.insert(binding, value);
        }
    }

    QList<ImportStatement::Ptr> *_imports;
    QString _source;
    // Stack of values of the current binding
    QStack<QString> _bindings;
    // Value of the current object being filled. Initially empty
    // it's created when the first object is found.
    CreatableQmlObject::Ptr _current;
    // Expression buffer, filled when needed (evaluating JS or binding)
    QStack<ExpressionBuffer::Ptr> _buffers;
};


// Document
struct QmlDocumentPrivate
{
    QmlObject::Ptr rootObject;
    QString fileName;
    QList<ImportStatement::Ptr> imports;
};

QmlDocument::QmlDocument()
    : d_ptr(new QmlDocumentPrivate)
{
}

QmlDocument::~QmlDocument()
{
}

QmlDocument::Ptr QmlDocument::create(const QString &fileName)
{
    QFile file (fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        return QmlDocument::Ptr();
    }

    QString source = QString::fromUtf8(file.readAll());
    file.close();

    QmlDocument::Ptr object = QmlDocument::Ptr(new QmlDocument());
    object->d_ptr->fileName = fileName;


    QmlJS::Engine engine;
    QmlJS::Lexer lexer (&engine);
    QmlJS::Parser parser (&engine);
    lexer.setCode(source, 1, true);
    parser.parse();

    QmlVisitor visitor;
    object->d_ptr->rootObject = visitor(source, object->d_ptr->imports, parser.rootNode());
    return object;
}

QString QmlDocument::fileName() const
{
    Q_D(const QmlDocument);
    return d->fileName;
}

QList<ImportStatement::Ptr> QmlDocument::imports() const
{
    Q_D(const QmlDocument);
    return d->imports;
}

QmlObject::Ptr QmlDocument::rootObject() const
{
    Q_D(const QmlDocument);
    return d->rootObject;
}
