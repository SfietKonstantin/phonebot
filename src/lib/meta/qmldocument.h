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

#ifndef QMLDOCUMENT_H
#define QMLDOCUMENT_H

#include <QtCore/QSharedPointer>
#include <QtCore/QMetaType>
#include <QtCore/QStringList>

class ImportStatementPrivate;
class ImportStatement
{
public:
    typedef QSharedPointer<ImportStatement> Ptr;
    typedef QSharedPointer<const ImportStatement> ConstPtr;
    virtual ~ImportStatement();
    static ImportStatement::Ptr create(const QString &importUri, const QString &importFile,
                                       const QString &version, const QString &importId);
    static ImportStatement::Ptr createImport(const QString &importUri, const QString &version,
                                             const QString &importId = QString());
    static ImportStatement::Ptr createFileImport(const QString &importFile,
                                                 const QString &version = QString(),
                                                 const QString &importId = QString());
    QString importUri() const;
    void setImportUri(const QString importUri);
    QString importFile() const;
    void setImportFile(const QString importFile);
    QString version() const;
    void setVersion(const QString importVersion);
    QString importId() const;
    void setImportId(const QString importId);
    QString toString() const;
protected:
    QScopedPointer<ImportStatementPrivate> d_ptr;
private:
    ImportStatement();
    Q_DECLARE_PRIVATE(ImportStatement)
};

class Expression
{
public:
    explicit Expression();
    explicit Expression(const QString &expression);
    QString value() const;
private:
    QString m_expression;
};

Q_DECLARE_METATYPE(Expression)

class Reference
{
public:
    explicit Reference();
    explicit Reference(const QString &identifier, const QStringList &fieldMembers = QStringList());
    QString identifier() const;
    QStringList fieldMembers() const;
    QString value() const;
private:
    QString m_identifier;
    QStringList m_fieldMembers;
};

Q_DECLARE_METATYPE(Reference)

class QmlObjectPrivate;
class QmlObject
{
public:
    typedef QSharedPointer<QmlObject> Ptr;
    typedef QSharedPointer<const QmlObject> ConstPtr;
    virtual ~QmlObject();
    static QmlObject::Ptr create(const QString &type);
    QString type() const;
    QString id() const;
    void setId(const QString &id);
    QStringList properties() const;
    bool hasProperty(const QString &key) const;
    QVariant property(const QString &key) const;
    void setProperties(const QVariantMap &properties);
    QList<QmlObject::Ptr> children() const;
    void setChildren(const QList<QmlObject::Ptr> &children);
    QString toString(int indentLevel) const;
protected:
    explicit QmlObject(QmlObjectPrivate &dd);
    QScopedPointer<QmlObjectPrivate> d_ptr;
private:
    QmlObject();
    Q_DECLARE_PRIVATE(QmlObject)
};

Q_DECLARE_METATYPE(QmlObject::Ptr)


class QmlDocumentBasePrivate;
class QmlDocumentBase
{
public:
    typedef QSharedPointer<QmlDocumentBase> Ptr;
    typedef QSharedPointer<const QmlDocumentBase> ConstPtr;
    virtual ~QmlDocumentBase();
    QList<ImportStatement::Ptr> imports() const;
    QmlObject::Ptr rootObject() const;
    QString toString() const;
protected:
    explicit QmlDocumentBase(QmlDocumentBasePrivate &dd);
    QScopedPointer<QmlDocumentBasePrivate> d_ptr;
private:
    explicit QmlDocumentBase();
    Q_DECLARE_PRIVATE(QmlDocumentBase)
};

class QmlDocumentPrivate;
class QmlDocument: public QmlDocumentBase
{
public:
    enum ErrorType {
        NoError,
        ParseError,
        NoRuleError,
        ComplexRuleError
    };
    typedef QSharedPointer<QmlDocument> Ptr;
    typedef QSharedPointer<const QmlDocument> ConstPtr;

    virtual ~QmlDocument();
    static Ptr create(const QString &fileName);
    static QmlDocumentBase::Ptr toBase(Ptr object);
    static QmlDocumentBase::ConstPtr toBase(ConstPtr object);
    QString fileName() const;
    ErrorType error() const;
    QString errorMessage() const;
private:
    explicit QmlDocument();
    Q_DECLARE_PRIVATE(QmlDocument)
};

class WritableQmlDocument: public QmlDocumentBase
{
public:
    typedef QSharedPointer<WritableQmlDocument> Ptr;
    typedef QSharedPointer<const WritableQmlDocument> ConstPtr;

    virtual ~WritableQmlDocument();
    static Ptr create();
    static QmlDocumentBase::Ptr toBase(Ptr object);
    static QmlDocumentBase::ConstPtr toBase(ConstPtr object);
    void clearImports();
    void addImport(ImportStatement::Ptr import);
    void setRootObject(QmlObject::Ptr rootObject);
private:
    explicit WritableQmlDocument();
    Q_DECLARE_PRIVATE(QmlDocumentBase)
};

#endif // QMLDOCUMENT_H
