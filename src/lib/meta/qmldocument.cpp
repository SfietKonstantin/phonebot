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
#include <QtCore/QTextStream>
#include "qmljsengine_p.h"
#include "qmljslexer_p.h"
#include "qmljsparser_p.h"

struct Error
{
    int line;
    int column;
    QString message;
};

static QString indent(int indentLevel)
{
    return QString("    ").repeated(indentLevel);
}

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

ImportStatement::~ImportStatement()
{
}

ImportStatement::Ptr ImportStatement::create(const QString &importUri, const QString &importFile,
                                             const QString &version, const QString &importId)
{
    ImportStatement::Ptr object = ImportStatement::Ptr(new ImportStatement);
    object->d_ptr->importUri = importUri;
    object->d_ptr->importFile = importFile;
    object->d_ptr->version = version;
    object->d_ptr->importId = importId;
    return object;
}

ImportStatement::Ptr ImportStatement::createImport(const QString &importUri,
                                                   const QString &version,
                                                   const QString &importId)
{
    ImportStatement::Ptr object = ImportStatement::Ptr(new ImportStatement);
    object->d_ptr->importUri = importUri;
    object->d_ptr->version = version;
    object->d_ptr->importId = importId;
    return object;
}

ImportStatement::Ptr ImportStatement::createFileImport(const QString &importFile,
                                                       const QString &version,
                                                       const QString &importId)
{
    ImportStatement::Ptr object = ImportStatement::Ptr(new ImportStatement);
    object->d_ptr->importFile = importFile;
    object->d_ptr->version = version;
    object->d_ptr->importId = importId;
    return object;
}

QString ImportStatement::importUri() const
{
    Q_D(const ImportStatement);
    return d->importUri;
}

void ImportStatement::setImportUri(const QString importUri)
{
    Q_D(ImportStatement);
    d->importUri = importUri;
}

QString ImportStatement::importFile() const
{
    Q_D(const ImportStatement);
    return d->importFile;
}

void ImportStatement::setImportFile(const QString importFile)
{
    Q_D(ImportStatement);
    d->importFile = importFile;
}

QString ImportStatement::version() const
{
    Q_D(const ImportStatement);
    return d->version;
}

void ImportStatement::setVersion(const QString version)
{
    Q_D(ImportStatement);
    d->version = version;
}

QString ImportStatement::importId() const
{
    Q_D(const ImportStatement);
    return d->importId;
}

void ImportStatement::setImportId(const QString importId)
{
    Q_D(ImportStatement);
    d->importId = importId;
}

QString ImportStatement::toString() const
{
    Q_D(const ImportStatement);
    QString out;
    QTextStream s (&out);
    s << "import ";
    if (!d->importUri.isEmpty()) {
        s << d->importUri;
    } else {
        s << "\"" << d->importFile << "\"";
    }
    if (!d->version.isEmpty()) {
        s << " " << d->version;
    }
    if (!d->importId.isEmpty()) {
        s << " as " << d->importId;
    }

    return out;
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

Reference::Reference()
{
}

Reference::Reference(const QString &identifier, const QStringList &fieldMembers)
    : m_identifier(identifier), m_fieldMembers(fieldMembers)
{
}

QString Reference::identifier() const
{
    return m_identifier;
}

QStringList Reference::fieldMembers() const
{
    return m_fieldMembers;
}

QString Reference::value() const
{
    QString value = m_identifier;
    foreach (const QString &identifier, m_fieldMembers) {
        value.append(QString(".%1").arg(identifier));
    }
    return value;
}

// Subclass exposing a creator + d-pointer
class CreatableQmlObject: public QmlObject
{
public:
    typedef QSharedPointer<CreatableQmlObject> Ptr;
    typedef QSharedPointer<const CreatableQmlObject> ConstPtr;
    explicit CreatableQmlObject();
    static CreatableQmlObject::Ptr create(const QString &type);
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

CreatableQmlObject::Ptr CreatableQmlObject::create(const QString &type)
{
    CreatableQmlObject::Ptr object = CreatableQmlObject::Ptr(new CreatableQmlObject());
    object->d_ptr->type = type;
    return object;
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

QmlObject::Ptr QmlObject::create(const QString &type)
{
    QmlObject::Ptr object = QmlObject::Ptr(new QmlObject());
    object->d_ptr->type = type;
    return object;
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

void QmlObject::setId(const QString &id)
{
    Q_D(QmlObject);
    d->id = id;
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

void QmlObject::setProperties(const QVariantMap &properties)
{
    Q_D(QmlObject);
    d->properties = properties;
}

QList<QmlObject::Ptr> QmlObject::children() const
{
    Q_D(const QmlObject);
    return d->children;
}

void QmlObject::setChildren(const QList<QmlObject::Ptr> &children)
{
    Q_D(QmlObject);
    d->children = children;
}

// TODO: migrate to MetaProperty
static QString formatProperty(const QString &key, const QVariant &value, int indentLevel)
{
    QString returned;
    bool ok = true;
    QTextStream ss (&returned);
    ss << indent(indentLevel + 1);
    if (!key.isEmpty()) {
        ss << key << ": ";
    }
    switch (value.type()) {
    case QMetaType::QString:
        ss << "\"" << value.toString() << "\"";
        break;
    case QMetaType::Int:
        ss << value.toInt();
        break;
    case QMetaType::Double:
        ss << value.toDouble();
        break;
    case QMetaType::Bool:
        ss << (value.toBool() ? "true" : "false");
        break;
    case QMetaType::QVariantList:
    {
        QStringList components;
        foreach (const QVariant &variant, value.toList()) {
            components.append(formatProperty(QString(), variant, indentLevel + 1));
        }
        ss << "[" << endl << components.join(",\n") << endl << indent(indentLevel + 1) << "]";
        break;
    }
    default:
        if (value.canConvert<QmlObject::Ptr>()) {
            QmlObject::Ptr object = value.value<QmlObject::Ptr>();
            if (!object.isNull()) {
                ss << object->toString(indentLevel + 1).trimmed();
            } else {
                ok = false;
            }
        } else if (value.canConvert<Reference>()) {
            ss << value.value<Reference>().value();
        }
        break;
    }

    if (!ok) {
        return QString();
    }
    return returned;
}

QString QmlObject::toString(int indentLevel) const
{
    Q_D(const QmlObject);
    QString out;
    QTextStream s (&out);
    s << d->type << " {" << endl;
    if (!d->id.isEmpty()) {
        s << indent(indentLevel + 1) << "id: " << d->id << endl;
    }

    for (QVariantMap::const_iterator i = d->properties.constBegin();
         i != d->properties.constEnd(); ++i) {
        QString property = formatProperty(i.key(), i.value(), indentLevel);
        if (!property.isEmpty()) {
            s << property << endl;
        }
    }

    foreach (QmlObject::Ptr object, d->children) {
        s << indent(indentLevel + 1) << object->toString(indentLevel + 1);
    }
    s << indent(indentLevel) << "}" << endl;
    return out;
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
    bool isArray;
private:
    ExpressionBuffer() : isArray(false) {}
};

// TODO: manage complex rules
class QmlVisitor: protected QmlJS::AST::Visitor
{
public:
    QmlDocument::ErrorType error;
    QList<Error> errors;

    QmlObject::Ptr operator()(const QString &source, QList<ImportStatement::Ptr> &imports,
                              QmlJS::AST::Node *node)
    {
        error = QmlDocument::NoError;
        errors.clear();
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
        if (error != QmlDocument::NoError) {
            return false;
        }

        QString importUri;
        if (ast->importUri) {
            importUri = ast->importUri->name.toString();
        }

        int offset = ast->versionToken.offset;
        int length = ast->versionToken.length;
        QString version = _source.mid(offset, length);

        QString fileName = ast->fileName.toString();
        QString importId = ast->importId.toString();

        ImportStatement::Ptr import = ImportStatement::create(importUri, fileName, version,
                                                              importId);
        _imports->append(import);
        return true;
    }

    // Manage bindings
    bool visit(QmlJS::AST::UiScriptBinding *ast)
    {
        if (error != QmlDocument::NoError) {
            return false;
        }

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
        if (error != QmlDocument::NoError) {
            return false;
        }

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
        bool ok = true;
        if (buffer->buffer.count() != 1) {
            ok = false;
        } else {
            QVariant first = buffer->buffer.first();
            if (first.canConvert<Reference>()) {
                Reference reference = first.value<Reference>();
                if (reference.value() != buffer->source) {
                    ok = false;
#ifdef QMLDOCUMENT_DEBUG
                    qDebug() << "Reference is" << reference.value() << "and source" << buffer->source;
#endif
                }
            }
        }


        if (!ok) {
            buffer->buffer.clear();
            buffer->buffer.append(QVariant::fromValue(Expression(buffer->source)));
        }
    }

    bool visit(QmlJS::AST::FieldMemberExpression *ast)
    {
        QString identifier;
        QStringList fieldMembers;
        QmlJS::AST::ExpressionNode *base = ast;
        while (base) {
            if (base->kind == QmlJS::AST::Node::Kind_FieldMemberExpression) {
                QmlJS::AST::FieldMemberExpression *astBase = static_cast<QmlJS::AST::FieldMemberExpression *>(base);
                fieldMembers.prepend(astBase->name.toString());
                base = astBase->base;
            } else if (base->kind == QmlJS::AST::Node::Kind_IdentifierExpression) {
                QmlJS::AST::IdentifierExpression *astBase = static_cast<QmlJS::AST::IdentifierExpression *>(base);
                identifier = astBase->name.toString();
                base = 0;
            } else {
                base = 0;
            }
        }

        Q_ASSERT(!identifier.isEmpty());
        Q_ASSERT(!fieldMembers.isEmpty());
        _buffers.top()->buffer.append(QVariant::fromValue(Reference(identifier, fieldMembers)));
        return false;
    }

    bool visit(QmlJS::AST::IdentifierExpression *ast)
    {
        _buffers.top()->buffer.append(QVariant::fromValue(Reference(ast->name.toString())));
        return true;
    }

    bool visit(QmlJS::AST::NumericLiteral *ast)
    {
        if (error != QmlDocument::NoError) {
            return false;
        }

        Q_ASSERT(!_bindings.isEmpty());
        _buffers.top()->buffer.append(ast->value);
        return true;
    }

    bool visit(QmlJS::AST::StringLiteral *ast)
    {
        if (error != QmlDocument::NoError) {
            return false;
        }

        Q_ASSERT(!_bindings.isEmpty());
        _buffers.top()->buffer.append(ast->value.toString());
        return true;
    }

    bool visit(QmlJS::AST::TrueLiteral *)
    {
        if (error != QmlDocument::NoError) {
            return false;
        }

        Q_ASSERT(!_bindings.isEmpty());
        _buffers.top()->buffer.append(QVariant(true));
        return true;
    }

    bool visit(QmlJS::AST::FalseLiteral *)
    {
        if (error != QmlDocument::NoError) {
            return false;
        }

        Q_ASSERT(!_bindings.isEmpty());
        _buffers.top()->buffer.append(QVariant(false));
        return true;
    }

    // Arrays
    bool visit(QmlJS::AST::UiArrayBinding *ast)
    {
        if (error != QmlDocument::NoError) {
            return false;
        }

        Q_ASSERT(ast->qualifiedId);
        visitBindings(ast->qualifiedId);

        // Push a second buffer for list
        ExpressionBuffer::Ptr buffer = ExpressionBuffer::create();
        buffer->isArray = true;
        _buffers.push(buffer);
        return true;
    }

    void endVisit(QmlJS::AST::UiArrayBinding *)
    {
        // Pop the first buffer and aggregate in the new buffer
        QVariantList buffers = _buffers.pop()->buffer;
        _buffers.top()->buffer.append(QVariant(buffers));
        endVisitBindings();
    }

    // Manage object bindings
    bool visit(QmlJS::AST::UiObjectBinding *ast)
    {
        if (error != QmlDocument::NoError) {
            return false;
        }

        Q_ASSERT(ast->qualifiedId);
        Q_ASSERT(ast->qualifiedTypeNameId);
        visitBindings(ast->qualifiedId);

        // Also push an object
        CreatableQmlObject::Ptr object = CreatableQmlObject::create(ast->qualifiedTypeNameId->name.toString());
        object->d()->parent = _current;
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
        if (error != QmlDocument::NoError) {
            return false;
        }

        CreatableQmlObject::Ptr object = CreatableQmlObject::create(ast->qualifiedTypeNameId->name.toString());
        object->d()->parent = _current;

        _current = object;
        return true;
    }

    void endVisit(QmlJS::AST::UiObjectDefinition *ast)
    {
        if (_current->d()->parent.isNull()) {
            // We are at root, let's check if the root is a Rule
            if (_current->type() != "Rule") {
                Error ruleError;
                ruleError.column = ast->firstSourceLocation().startColumn;
                ruleError.line = ast->firstSourceLocation().startLine;
                ruleError.message = "Error: root component type is not Rule";
                errors.append(ruleError);
                error = QmlDocument::NoRuleError;
            }

            return;
        }

        bool inArray = false;
        if (!_buffers.isEmpty()) {
            inArray = _buffers.top()->isArray;
        }
        if (!inArray) {
            _current->d()->parent->d()->children.append(_current);
        } else {
            _buffers.top()->buffer.append(QVariant::fromValue(CreatableQmlObject::toObject(_current)));
        }
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
        if (binding == "id" && value.canConvert<Reference>()) {
            Reference id = value.value<Reference>();
            _current->setId(id.identifier());
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
struct QmlDocumentBasePrivate
{
    explicit QmlDocumentBasePrivate();
    QmlObject::Ptr rootObject;
    QList<ImportStatement::Ptr> imports;
};

QmlDocumentBasePrivate::QmlDocumentBasePrivate()
{
}

QmlDocumentBase::QmlDocumentBase()
    : d_ptr(new QmlDocumentBasePrivate)
{
}

QmlDocumentBase::QmlDocumentBase(QmlDocumentBasePrivate &dd)
    : d_ptr(&dd)
{
}

QmlDocumentBase::~QmlDocumentBase()
{
}

QList<ImportStatement::Ptr> QmlDocumentBase::imports() const
{
    Q_D(const QmlDocumentBase);
    return d->imports;
}

QmlObject::Ptr QmlDocumentBase::rootObject() const
{
    Q_D(const QmlDocumentBase);
    return d->rootObject;
}

QString QmlDocumentBase::toString() const
{
    Q_D(const QmlDocumentBase);
    QString out;
    QTextStream s (&out);
    foreach (ImportStatement::ConstPtr import, d->imports) {
        s << import->toString() << endl;
    }

    s << endl;
    s << d->rootObject->toString(0);
    return out;
}

struct QmlDocumentPrivate: public QmlDocumentBasePrivate
{
    explicit QmlDocumentPrivate();
    QString fileName;
    QmlDocument::ErrorType error;
    QString errorMessage;
};

QmlDocumentPrivate::QmlDocumentPrivate()
    : QmlDocumentBasePrivate(), error(QmlDocument::NoError)
{
}

QmlDocument::QmlDocument()
    : QmlDocumentBase(*(new QmlDocumentPrivate))
{
}

QmlDocument::~QmlDocument()
{
}

static QString formatError(int line, int column, const QString &error)
{
    return QString("l%1 c%2: %3\n").arg(QString::number(line), QString::number(column), error);
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
    object->d_func()->fileName = fileName;


    QmlJS::Engine engine;
    QmlJS::Lexer lexer (&engine);
    QmlJS::Parser parser (&engine);
    lexer.setCode(source, 1, true);
    parser.parse();

    const QList<QmlJS::DiagnosticMessage> diagnosticMessages = parser.diagnosticMessages();
    if (diagnosticMessages.count() > 0) {
        foreach (const QmlJS::DiagnosticMessage &message, diagnosticMessages) {
            if (message.isError()) {
                object->d_func()->error = QmlDocument::ParseError;
                object->d_func()->errorMessage.append(formatError(message.loc.startLine,
                                                                  message.loc.startColumn,
                                                                  message.message));
            }
        }
    }

    if (object->d_func()->error == QmlDocument::NoError) {
        QmlVisitor visitor;
        object->d_func()->rootObject = visitor(source, object->d_func()->imports, parser.rootNode());
        object->d_func()->error = visitor.error;

        foreach (const Error &error, visitor.errors) {
            object->d_func()->errorMessage.append(formatError(error.line, error.column, error.message));
        }
    }
    object->d_func()->errorMessage = object->d_func()->errorMessage.trimmed();
    return object;
}

QmlDocumentBase::Ptr QmlDocument::toBase(Ptr object)
{
    return qSharedPointerCast<QmlDocumentBase>(object);
}

QmlDocumentBase::ConstPtr QmlDocument::toBase(ConstPtr object)
{
    return qSharedPointerCast<const QmlDocumentBase>(object);
}

QString QmlDocument::fileName() const
{
    Q_D(const QmlDocument);
    return d->fileName;
}

QmlDocument::ErrorType QmlDocument::error() const
{
    Q_D(const QmlDocument);
    return d->error;
}

QString QmlDocument::errorMessage() const
{
    Q_D(const QmlDocument);
    return d->errorMessage;
}

WritableQmlDocument::WritableQmlDocument()
    : QmlDocumentBase(*(new QmlDocumentPrivate))
{
}

WritableQmlDocument::~WritableQmlDocument()
{
}

WritableQmlDocument::Ptr WritableQmlDocument::create()
{
    WritableQmlDocument::Ptr object = WritableQmlDocument::Ptr(new WritableQmlDocument());
    return object;
}

QmlDocumentBase::Ptr WritableQmlDocument::toBase(Ptr object)
{
    return qSharedPointerCast<QmlDocumentBase>(object);
}

QmlDocumentBase::ConstPtr WritableQmlDocument::toBase(ConstPtr object)
{
    return qSharedPointerCast<const QmlDocumentBase>(object);
}

void WritableQmlDocument::clearImports()
{
    Q_D(QmlDocumentBase);
    d->imports.clear();
}

void WritableQmlDocument::addImport(ImportStatement::Ptr import)
{
    Q_D(QmlDocumentBase);
    d->imports.append(import);
}

void WritableQmlDocument::setRootObject(QmlObject::Ptr rootObject)
{
    Q_D(QmlDocumentBase);
    d->rootObject = rootObject;
}
