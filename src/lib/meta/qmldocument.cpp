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
#include <QtQml/private/qqmljsengine_p.h>
#include <QtQml/private/qqmljslexer_p.h>
#include <QtQml/private/qqmljsparser_p.h>

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
    return ImportStatement::create(importUri, QString(), version, importId);
}

ImportStatement::Ptr ImportStatement::createFileImport(const QString &importFile,
                                                       const QString &version,
                                                       const QString &importId)
{
    return ImportStatement::create(QString(), importFile, version, importId);
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

Expression::Expression(const QString &expression)
    : m_expression(expression)
{
}

Expression::~Expression()
{
}

Expression::Ptr Expression::create(const QString &expression)
{
    return Ptr(new Expression(expression));
}

QString Expression::value() const
{
    return m_expression;
}

Reference::Reference(const QString &identifier, const QStringList &fieldMembers)
    : m_identifier(identifier), m_fieldMembers(fieldMembers)
{
}

Reference::~Reference()
{
}

Reference::Ptr Reference::create(const QString &identifier, const QStringList &fieldMembers)
{
    return Ptr(new Reference(identifier, fieldMembers));
}

QString Reference::identifier() const
{
    return m_identifier;
}

QString Reference::value() const
{
    QString value = m_identifier;
    for (const QString &identifier : m_fieldMembers) {
        value.append(QString(".%1").arg(identifier));
    }
    return value;
}

// Subclass exposing a creator + d-pointer
class CreatableQmlObject: public QmlObject
{
public:
    typedef QSharedPointer<CreatableQmlObject> Ptr;
    static CreatableQmlObject::Ptr create(const QString &type);
    static QmlObject::Ptr toObject(Ptr creatable);
    QmlObjectPrivate * d();
private:
    explicit CreatableQmlObject();
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
        for (const QVariant &variant : value.toList()) {
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
        } else if (value.canConvert<Reference::Ptr>()) {
            ss << value.value<Reference::Ptr>()->value();
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

    for (QmlObject::Ptr object : d->children) {
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
class QmlVisitor: protected QQmlJS::AST::Visitor
{
public:
    QmlDocument::ErrorType error;
    QList<Error> errors;

    QmlObject::Ptr operator()(const QString &source, QList<ImportStatement::Ptr> &imports,
                              QQmlJS::AST::Node *node)
    {
        error = QmlDocument::NoError;
        errors.clear();
        m_imports = &imports;
        m_imports->clear();
        m_source = source;
        QQmlJS::AST::Node::accept(node, this);
        return m_current;
    }
protected:
#ifdef QMLDOCUMENT_DEBUG
    bool preVisit(QQmlJS::AST::Node *ast)
    {
        qDebug() << "Begin:" << ast->kind;
        qDebug() << "Full:";
        int offset = ast->firstSourceLocation().offset;
        int length = ast->lastSourceLocation().offset + ast->lastSourceLocation().length - offset;
        qDebug() << m_source.mid(offset, length);
        qDebug() << "First";
        length = ast->firstSourceLocation().length;
        qDebug() << m_source.mid(offset, length);
        return true;
    }

    void postVisit(QQmlJS::AST::Node *ast)
    {
        qDebug() << "End:  " << ast->kind;
    }
#endif

    // Manage the import
    bool visit(QQmlJS::AST::UiImport *ast)
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
        QString version = m_source.mid(offset, length);

        QString fileName = ast->fileName.toString();
        QString importId = ast->importId.toString();

        ImportStatement::Ptr import = ImportStatement::create(importUri, fileName, version,
                                                              importId);
        m_imports->append(import);
        return true;
    }

    // Manage bindings
    bool visit(QQmlJS::AST::UiScriptBinding *ast)
    {
        if (error != QmlDocument::NoError) {
            return false;
        }

        Q_ASSERT(ast->qualifiedId);
        visitBindings(ast->qualifiedId);
        return true;
    }

    void endVisit(QQmlJS::AST::UiScriptBinding *)
    {
        endVisitBindings();
    }

    bool visit(QQmlJS::AST::ExpressionStatement *ast)
    {
        if (error != QmlDocument::NoError) {
            return false;
        }

        Q_ASSERT(!m_buffers.isEmpty());
        ExpressionBuffer::Ptr buffer = m_buffers.top();

        int offset = ast->firstSourceLocation().offset;
        int length = ast->lastSourceLocation().offset + ast->lastSourceLocation().length - offset;
        buffer->source = m_source.mid(offset, length);
        return true;
    }

    void endVisit(QQmlJS::AST::ExpressionStatement *)
    {
        Q_ASSERT(!m_buffers.isEmpty());
        ExpressionBuffer::Ptr buffer = m_buffers.top();

        // If we found an unparsable entry, we just put
        // the source
        bool ok = true;
        if (buffer->buffer.count() != 1) {
            ok = false;
        } else {
            QVariant first = buffer->buffer.first();
            if (first.canConvert<Reference::Ptr>()) {
                Reference::Ptr reference = first.value<Reference::Ptr>();
                if (reference->value() != buffer->source) {
                    ok = false;
#ifdef QMLDOCUMENT_DEBUG
                    qDebug() << "Reference is" << reference->value() << "and source" << buffer->source;
#endif
                }
            }
        }


        if (!ok) {
            buffer->buffer.clear();
            buffer->buffer.append(QVariant::fromValue(Expression::create(buffer->source)));
        }
    }

    bool visit(QQmlJS::AST::FieldMemberExpression *ast)
    {
        QString identifier;
        QStringList fieldMembers;
        QQmlJS::AST::ExpressionNode *base = ast;
        while (base != nullptr) {
            if (base->kind == QQmlJS::AST::Node::Kind_FieldMemberExpression) {
                QQmlJS::AST::FieldMemberExpression *astBase = static_cast<QQmlJS::AST::FieldMemberExpression *>(base);
                fieldMembers.prepend(astBase->name.toString());
                base = astBase->base;
            } else if (base->kind == QQmlJS::AST::Node::Kind_IdentifierExpression) {
                QQmlJS::AST::IdentifierExpression *astBase = static_cast<QQmlJS::AST::IdentifierExpression *>(base);
                identifier = astBase->name.toString();
                base = nullptr;
            } else {
                base = nullptr;
            }
        }

        Q_ASSERT(!identifier.isEmpty());
        Q_ASSERT(!fieldMembers.isEmpty());
        m_buffers.top()->buffer.append(QVariant::fromValue(Reference::create(identifier, fieldMembers)));
        return false;
    }

    bool visit(QQmlJS::AST::IdentifierExpression *ast)
    {
        m_buffers.top()->buffer.append(QVariant::fromValue(Reference::create(ast->name.toString())));
        return true;
    }

    bool visit(QQmlJS::AST::NumericLiteral *ast)
    {
        if (error != QmlDocument::NoError) {
            return false;
        }

        Q_ASSERT(!m_bindings.isEmpty());
        m_buffers.top()->buffer.append(ast->value);
        return true;
    }

    bool visit(QQmlJS::AST::StringLiteral *ast)
    {
        if (error != QmlDocument::NoError) {
            return false;
        }

        Q_ASSERT(!m_bindings.isEmpty());
        m_buffers.top()->buffer.append(ast->value.toString());
        return true;
    }

    bool visit(QQmlJS::AST::TrueLiteral *)
    {
        if (error != QmlDocument::NoError) {
            return false;
        }

        Q_ASSERT(!m_bindings.isEmpty());
        m_buffers.top()->buffer.append(QVariant(true));
        return true;
    }

    bool visit(QQmlJS::AST::FalseLiteral *)
    {
        if (error != QmlDocument::NoError) {
            return false;
        }

        Q_ASSERT(!m_bindings.isEmpty());
        m_buffers.top()->buffer.append(QVariant(false));
        return true;
    }

    // Arrays
    bool visit(QQmlJS::AST::UiArrayBinding *ast)
    {
        if (error != QmlDocument::NoError) {
            return false;
        }

        Q_ASSERT(ast->qualifiedId);
        visitBindings(ast->qualifiedId);

        // Push a second buffer for list
        ExpressionBuffer::Ptr buffer = ExpressionBuffer::create();
        buffer->isArray = true;
        m_buffers.push(buffer);
        return true;
    }

    void endVisit(QQmlJS::AST::UiArrayBinding *)
    {
        // Pop the first buffer and aggregate in the new buffer
        QVariantList buffers = m_buffers.pop()->buffer;
        m_buffers.top()->buffer.append(QVariant(buffers));
        endVisitBindings();
    }

    // Manage object bindings
    bool visit(QQmlJS::AST::UiObjectBinding *ast)
    {
        if (error != QmlDocument::NoError) {
            return false;
        }

        Q_ASSERT(ast->qualifiedId);
        Q_ASSERT(ast->qualifiedTypeNameId);
        visitBindings(ast->qualifiedId);

        // Also push an object
        CreatableQmlObject::Ptr object = CreatableQmlObject::create(ast->qualifiedTypeNameId->name.toString());
        object->d()->parent = m_current;
        m_current = object;

        // Directly insert the object inside the buffer
        m_buffers.top()->buffer.append(QVariant::fromValue(CreatableQmlObject::toObject(object)));
        return true;
    }

    void endVisit(QQmlJS::AST::UiObjectBinding *)
    {
        Q_ASSERT(!m_current->d()->parent.isNull());
        CreatableQmlObject::Ptr parent = m_current->d()->parent;
        m_current->d()->parent.clear(); // Remove circular references on children and parent
        m_current = parent;
        endVisitBindings();
    }

    // Component management
    bool visit(QQmlJS::AST::UiObjectDefinition *ast)
    {
        if (error != QmlDocument::NoError) {
            return false;
        }

        CreatableQmlObject::Ptr object = CreatableQmlObject::create(ast->qualifiedTypeNameId->name.toString());
        object->d()->parent = m_current;

        m_current = object;
        return true;
    }

    void endVisit(QQmlJS::AST::UiObjectDefinition *ast)
    {
        if (m_current->d()->parent.isNull()) {
            // We are at root, let's check if the root is a Rule
            if (m_current->type() != "Rule") {
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
        if (!m_buffers.isEmpty()) {
            inArray = m_buffers.top()->isArray;
        }
        if (!inArray) {
            m_current->d()->parent->d()->children.append(m_current);
        } else {
            m_buffers.top()->buffer.append(QVariant::fromValue(CreatableQmlObject::toObject(m_current)));
        }
        CreatableQmlObject::Ptr parent = m_current->d()->parent;
        m_current->d()->parent.clear(); // Remove circular references on children and parent
        m_current = parent;
    }

private:
    // Helper functions
    void visitBindings(QQmlJS::AST::UiQualifiedId *key)
    {
        int offset = key->firstSourceLocation().offset;
        int length = key->lastSourceLocation().offset + key->lastSourceLocation().length - offset;
        m_bindings.push(m_source.mid(offset, length));

        // Also push a buffer
        m_buffers.push(ExpressionBuffer::create());
    }

    void endVisitBindings()
    {
        Q_ASSERT(!m_buffers.isEmpty());
        Q_ASSERT(!m_bindings.isEmpty());
        Q_ASSERT(!m_current.isNull());
        QString binding = m_bindings.pop();
        ExpressionBuffer::Ptr buffer = m_buffers.pop();
        Q_ASSERT(buffer->buffer.count() == 1);

        QVariant value = buffer->buffer.first();
        if (binding == "id" && value.canConvert<Reference::Ptr>()) {
            Reference::Ptr id = value.value<Reference::Ptr>();
            m_current->setId(id->identifier());
        } else {
            m_current->d()->properties.insert(binding, value);
        }
    }

    QList<ImportStatement::Ptr> *m_imports;
    QString m_source;
    // Stack of values of the current binding
    QStack<QString> m_bindings;
    // Value of the current object being filled. Initially empty
    // it's created when the first object is found.
    CreatableQmlObject::Ptr m_current;
    // Expression buffer, filled when needed (evaluating JS or binding)
    QStack<ExpressionBuffer::Ptr> m_buffers;

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
    for (ImportStatement::ConstPtr import : d->imports) {
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


    QQmlJS::Engine engine;
    QQmlJS::Lexer lexer (&engine);
    QQmlJS::Parser parser (&engine);
    lexer.setCode(source, 1, true);
    parser.parse();

    const QList<QQmlJS::DiagnosticMessage> diagnosticMessages = parser.diagnosticMessages();
    if (diagnosticMessages.count() > 0) {
        for (const QQmlJS::DiagnosticMessage &message : diagnosticMessages) {
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

        for (const Error &error : visitor.errors) {
            object->d_func()->errorMessage.append(formatError(error.line, error.column, error.message));
        }
    }
    object->d_func()->errorMessage = object->d_func()->errorMessage.trimmed();
    return object;
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
