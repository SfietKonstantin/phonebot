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

#include "metatypecache.h"
#include "abstractmetadata.h"
#include <QtCore/QDebug>
#include <QtCore/QGlobalStatic>
#include <QtCore/QMetaClassInfo>
#include <QtCore/QMetaType>
#include <QtQml/private/qqmlmetatype_p.h>

static const char *TRIGGER_META = "Trigger";
static const char *CONDITION_META = "Condition";
static const char *ACTION_META = "Action";
static const char *JSCONDITION_META  ="JsCondition";
static const char *JSACTION_META  ="JsAction";

static const char *NO_METADATA_MACRO = "NO_METADATA";

struct SortingMetaDataInfo
{
    SortingMetaDataInfo(const QString &type, AbstractMetaData *metaData)
        : type(type), metaData(metaData)
    {
    }
    QString type;
    AbstractMetaData *metaData;
};

static bool isFilteredOut(const QByteArray &className)
{
    if (className == JSCONDITION_META || className == JSACTION_META || className == TRIGGER_META) {
        return true;
    }

    return false;
}

static MetaTypeCache::Type isComponent(const QByteArray &className)
{
    if (className == TRIGGER_META) {
        return MetaTypeCache::Trigger;
    } else if (className == CONDITION_META) {
        return MetaTypeCache::Condition;
    } else if (className == ACTION_META) {
        return MetaTypeCache::Action;
    } else {
        return MetaTypeCache::Invalid;
    }
}

struct QmlVersion
{
    explicit QmlVersion() : majorVersion(-1), minorVersion(-1) {}
    explicit QmlVersion(int majorVersion, int minorVersion)
        : majorVersion(majorVersion), minorVersion(minorVersion)
    {
    }
    QmlVersion(const QmlVersion &other)
        : majorVersion(other.majorVersion), minorVersion(other.minorVersion)
    {
    }
    QmlVersion & operator=(const QmlVersion &other)
    {
        majorVersion = other.majorVersion;
        minorVersion = other.minorVersion;
        return *this;
    }
    bool operator<(const QmlVersion &other) const
    {
        if (majorVersion == other.majorVersion) {
            return minorVersion < other.minorVersion;
        }
        return majorVersion < other.majorVersion;
    }
    bool operator==(const QmlVersion &other) const
    {
        return majorVersion == other.majorVersion && minorVersion == other.minorVersion;
    }
    int majorVersion;
    int minorVersion;
};

struct MetaTypeCacheItem
{
    explicit MetaTypeCacheItem(const QMetaObject *metaObject)
        : metaData(0), metaObject(metaObject), majorVersion(-1), minorVersion(-1)
    {
    }
    ~MetaTypeCacheItem()
    {
        if (metaData) {
            metaData->deleteLater();
        }
    }
    AbstractMetaData *metaData;
    const QMetaObject *metaObject;
    QStringList properties;
    QString module;
    int majorVersion;
    int minorVersion;
    MetaTypeCache::Type type;
};





class MetaTypeCachePrivate
{
public:
    explicit MetaTypeCachePrivate(MetaTypeCache *q);
    void populateCache();
    void populateCacheForType(const QQmlType *type, const QString &module,
                              int majorVersion, int minorVersion);
    QMap<QString, MetaTypeCacheItem *> metaCache;
protected:
    MetaTypeCache * const q_ptr;
private:
    Q_DECLARE_PUBLIC(MetaTypeCache)
};

MetaTypeCachePrivate::MetaTypeCachePrivate(MetaTypeCache *q)
    : q_ptr(q)
{
}

void MetaTypeCachePrivate::populateCache()
{
    QList<QmlVersion> versions;

    // Dirty hack
    // 1. parse QQmlType, since you can extract version info from it.
    // 2. parse type name, since you can get the module. Use all versions
    //    to be able to get the module + version matching for a given component.
    for (const QQmlType *type : QQmlMetaType::qmlTypes()) {
        QmlVersion version (type->majorVersion(), type->minorVersion());
        if (version.majorVersion != -1 && version.minorVersion != -1 && !versions.contains(version)) {
            versions.append(version);
        }
    }

    std::sort(versions.begin(), versions.end(), std::less<QmlVersion>());

    for (const QString &typeName : QQmlMetaType::qmlTypeNames()) {
        QQmlType *type = 0;
        int majorVersion = -1;
        int minorVersion = -1;
        QList<QmlVersion>::iterator i = versions.begin();
        while (i != versions.end() && !type) {
            majorVersion = i->majorVersion;
            minorVersion = i->minorVersion;
            type = QQmlMetaType::qmlType(typeName, majorVersion, minorVersion);
            ++i;
        }


        if (type) {
            QStringList moduleAndType = typeName.split("/");
            if (moduleAndType.count() == 2) {
                populateCacheForType(type, moduleAndType.first(), majorVersion, minorVersion);
            }
        }
    }
}

void MetaTypeCachePrivate::populateCacheForType(const QQmlType *type, const QString &module,
                                                int majorVersion, int minorVersion)
{
    if (!type->isCreatable()) {
        return;
    }

    QByteArray typeName = type->typeName();
    typeName.append("*");
    const char *cppType = QMetaObject::normalizedType(typeName).constData();
    QString componentType = type->elementName();

    // 1. Try to get the Phonebot MetaData
    int componentTypeIndex = QMetaType::type(cppType);
    if (componentTypeIndex == QMetaType::UnknownType) {
        qWarning() << "Cannot get Phonebot metadata from" << componentType << ":"
                   << "Cannot find Qt metatype for" << cppType
                   << "It might not be registered via qmlRegisterType.";
        return;
    }

    // 2. Check if we are getting a Trigger, a Condition or an Action
    const QMetaObject *componentMeta = QMetaType::metaObjectForType(componentTypeIndex);
    if (isFilteredOut(componentMeta->className())) {
        return;
    }

    MetaTypeCache::Type typeEnum = MetaTypeCache::Invalid;
    const QMetaObject *super = componentMeta;
    while (super) {
        if (typeEnum == MetaTypeCache::Invalid) {
            typeEnum = isComponent(super->className());
        }
        super = super->superClass();
    }

    if (typeEnum == MetaTypeCache::Invalid) {
        return;
    }

    int metaClassInfoIndex = componentMeta->indexOfClassInfo("meta");
    if (metaClassInfoIndex == -1) {
        qWarning() << "Cannot get Phonebot metadata from" << componentType << ":"
                   << cppType << "don't have a Phonebot metatype."
                   << "Did you forgot PHONEBOT_METADATA macro ?";
        return;
    }

    QMetaClassInfo metaClassInfo = componentMeta->classInfo(metaClassInfoIndex);
    QByteArray metaNameByteArray (metaClassInfo.value());
    if (metaNameByteArray == NO_METADATA_MACRO) {
        return;
    }

    metaNameByteArray.append("*");
    const char *metaName = QMetaObject::normalizedType(metaNameByteArray).constData();

    int metaTypeIndex = QMetaType::type(metaName);
    if (metaTypeIndex == QMetaType::UnknownType) {
        qWarning() << "Cannot get Phonebot metadata from" << componentType << ":"
                   << "Cannot find Qt metatype for Phonebot metatype" << metaName
                   << "It might not be registered via qDeclareMetaType.";
        return;
    }

    // 3. Check if it inherits from AbstractMetaData
    bool ok = false;
    const QMetaObject *componentMetaMeta = QMetaType::metaObjectForType(metaTypeIndex);
    super = componentMetaMeta;
    while (super) {
        if (!ok) {
            ok =  (super->className() == AbstractMetaData::staticMetaObject.className());
        }
        super = super->superClass();
    }

    if (!ok) {
        qWarning() << "Cannot get Phonebot metadata from" << componentType << ":"
                   << "Phonebot metatype class" << metaName << "don't inherit from"
                   << AbstractMetaData::staticMetaObject.className();
        return;
    }

    // 4. Construct
    QObject *meta = componentMetaMeta->newInstance();
    if (!meta) {
        qWarning() << "Cannot get Phonebot metadata from" << componentType << ":"
                   << "Phonebot metatype class " << metaName
                   << "don't have an invokable constructor and cannot be created.";
        return;
    }
    AbstractMetaData *castedMeta = qobject_cast<AbstractMetaData *>(meta);
    Q_ASSERT(castedMeta);

    QStringList properties;
    for (int i = componentMeta->propertyOffset(); i < componentMeta->propertyCount(); ++i) {
        QString property = componentMeta->property(i).name();
        if (!properties.contains(property)) {
            properties.append(property);
        }
    }

    QStringList removedProperties;
    for (const QString &property : properties) {
        if (!castedMeta->property(property)) {
            qWarning() << property << "do not have metadata registered";
        }
    }

    for (const QString &property : removedProperties) {
        properties.removeAll(property);
    }

    MetaTypeCacheItem *item = new MetaTypeCacheItem(componentMeta);
    item->metaData = castedMeta;
    item->properties = properties;
    item->module = module;
    item->majorVersion = majorVersion;
    item->minorVersion = minorVersion;
    item->type = typeEnum;
    metaCache.insert(componentType, item);
}

MetaTypeCache::MetaTypeCache(QObject *parent) :
    QObject(parent), d_ptr(new MetaTypeCachePrivate(this))
{
    Q_D(MetaTypeCache);
    d->populateCache();
}

MetaTypeCache::~MetaTypeCache()
{
    Q_D(MetaTypeCache);
    qDeleteAll(d->metaCache);
}

bool MetaTypeCache::exists(const QString &type) const
{
    Q_D(const MetaTypeCache);
    return d->metaCache.contains(type);
}

AbstractMetaData * MetaTypeCache::metaData(const QString &type) const
{
    Q_D(const MetaTypeCache);
    if (!d->metaCache.contains(type)) {
        return 0;
    }

    return d->metaCache.value(type)->metaData;
}

const QMetaObject * MetaTypeCache::metaObject(const QString &type) const
{
    Q_D(const MetaTypeCache);
    if (!d->metaCache.contains(type)) {
        return 0;
    }

    return d->metaCache.value(type)->metaObject;
}

QStringList MetaTypeCache::properties(const QString &type) const
{
    Q_D(const MetaTypeCache);
    if (!d->metaCache.contains(type)) {
        return QStringList();
    }

    return d->metaCache.value(type)->properties;
}

QStringList MetaTypeCache::components(Type type) const
{
    Q_D(const MetaTypeCache);
    QList<SortingMetaDataInfo> componentsMeta;
    for (MetaTypeCacheItem *item : d->metaCache) {
        if (type == item->type) {
            componentsMeta.append(SortingMetaDataInfo (item->metaObject->className(), item->metaData));
        }
    }

    std::sort(componentsMeta.begin(), componentsMeta.end(),
              [](const SortingMetaDataInfo &metaData1, const SortingMetaDataInfo &metaData2){
        return metaData1.metaData->name() < metaData2.metaData->name();
    });
    QStringList components;
    for (const SortingMetaDataInfo &info : componentsMeta) {
        components.append(info.type);
    }

    return components;
}

ImportStatement::Ptr MetaTypeCache::import(const QString &type) const
{
    Q_D(const MetaTypeCache);
    if (!d->metaCache.contains(type)) {
        return ImportStatement::Ptr();
    }

    MetaTypeCacheItem *item = d->metaCache.value(type);
    return ImportStatement::createImport(item->module,
                                         QString("%1.%2").arg(QString::number(item->majorVersion),
                                                              QString::number(item->minorVersion)));
}
