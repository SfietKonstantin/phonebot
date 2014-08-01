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

#include "enginemanager.h"
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QStandardPaths>
#include "adaptor.h"

static const char *SERVICE = "org.SfietKonstantin.phonebot";
static const char *ROOT = "/";

static const char *RULE_FILE = "rule.qml";
static const char *DIR_FORMAT = "rule_%1";

class EngineManagerPrivate
{
public:
    explicit EngineManagerPrivate(EngineManager *q);
    void slotComponentLoadingFinished(const QUrl &url, bool ok);
    bool registerToBus();
    void unregisterFromBus();
    bool running;
    PhoneBotEngine *engine;
    QStringList rules;
    QSet<QUrl> loadingComponents;
protected:
    EngineManager * const q_ptr;
private:
    Q_DECLARE_PUBLIC(EngineManager)
};

EngineManagerPrivate::EngineManagerPrivate(EngineManager *q)
    : running(false), engine(0), q_ptr(q)
{
}

bool EngineManagerPrivate::registerToBus()
{
    Q_Q(EngineManager);
    QDBusConnection connection = QDBusConnection::sessionBus();
    if (!connection.registerService(SERVICE)) {
        return false;
    }
    return connection.registerObject(ROOT, q);
}

void EngineManagerPrivate::unregisterFromBus()
{
    QDBusConnection connection = QDBusConnection::sessionBus();
    connection.unregisterObject(ROOT);
    connection.unregisterService(SERVICE);
}

void EngineManagerPrivate::slotComponentLoadingFinished(const QUrl &url, bool ok)
{
    Q_UNUSED(ok)

    // If it's empty, it means that stop has been called
    // Don't try to launch engine
    if (loadingComponents.isEmpty()) {
        return;
    }

    loadingComponents.remove(url);
    if (loadingComponents.isEmpty()) {
        engine->start();
        running = true;
    }
}

EngineManager::EngineManager(QObject *parent)
    : QObject(parent), d_ptr(new EngineManagerPrivate(this))
{
    Q_D(EngineManager);
    d->engine = new PhoneBotEngine(this);
    d->engine->registerTypes();
    connect(d->engine, SIGNAL(componentLoadingFinished(QUrl,bool)),
            this, SLOT(slotComponentLoadingFinished(QUrl,bool)));
    new PhonebotAdaptor(this);
    if (!d->registerToBus()) {
        qWarning() << "Failed to register to DBus. Maybe another daemon is running";
    }
}

EngineManager::~EngineManager()
{
    Q_D(EngineManager);
    stop();
    d->unregisterFromBus();
}

bool EngineManager::isRunning() const
{
    Q_D(const EngineManager);
    return d->running;
}

QStringList EngineManager::rules() const
{
    Q_D(const EngineManager);
    return d->rules;
}

static QString generateDirName(int index)
{
    QString indexStr = QString::number(index);
    int nZeros = qMax(5 - indexStr.count(), 0);
    QString number = QString("0").repeated(nZeros);
    number.append(indexStr);
    return QString(DIR_FORMAT).arg(number);
}

bool EngineManager::addRule(const QString &rule)
{
    QString configRoot = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    configRoot.append(QString("/%1/%2/").arg(QCoreApplication::instance()->organizationName(),
                                             QCoreApplication::instance()->applicationName()));
    QDir dir (configRoot);
    QStringList dirs = dir.entryList(QDir::Dirs);
    QSet<QString> dirsSet = dirs.toSet();

    int index = 0;
    QString dirName = generateDirName(index);
    while (dirsSet.contains(dirName)) {
        ++ index;
        dirName = generateDirName(index);
    }

    if (!dir.mkpath(dirName)) {
        qWarning() << "Failed to create directory for new rule";
        qWarning() << "Creating directory" << dirName << "in" << dir.absolutePath();
        return false;
    }

    if (!dir.cd(dirName)) {
        qWarning() << "Failed to enter in created directory for new rule";
        qWarning() << "Enter in" << dirName << "from" << dir.absolutePath();
        return false;
    }

    QFile file (dir.absoluteFilePath(RULE_FILE));
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open file to write new rule";
        qWarning() << "File:" << dir.absoluteFilePath(RULE_FILE);
        return false;
    }

    file.write(rule.toLocal8Bit());
    file.close();

    reloadEngine();
    return true;
}

bool EngineManager::removeRule(const QString &path)
{
    QFileInfo info (path);
    if (!info.exists()) {
        return false;
    }

    if (!info.isFile()) {
        return false;
    }

    QDir folder = info.absoluteDir();
    if (!folder.remove(info.fileName())) {
        return false;
    }

    bool ok = true;
    if (folder.entryList(QDir::AllEntries | QDir::System |QDir::NoDotAndDotDot).isEmpty()) {
        ok = folder.removeRecursively();
    }
    reloadEngine();
    return ok;
}

bool EngineManager::editRule(const QString &path, const QString &rule)
{
    QFileInfo info (path);
    if (!info.exists()) {
        return false;
    }

    QFile file (path);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open file to edit rule";
        return false;
    }

    file.write(rule.toLocal8Bit());
    file.close();

    reloadEngine();
    return true;
}

void EngineManager::reloadEngine()
{
    Q_D(EngineManager);
    stop();
    QStringList rules = d->rules;
    d->rules.clear();

    // We check every folder inside .config/<org>/<app>/
    // and see if there is a "rule.qml" inside
    QString configRoot = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    configRoot.append(QString("/%1/%2/").arg(QCoreApplication::instance()->organizationName(),
                                             QCoreApplication::instance()->applicationName()));
    qDebug() << "Using" << configRoot << "to search rules";
    QDir dir (configRoot);
    foreach (const QString &subdirPath, dir.entryList(QDir::Dirs)) {
        QDir subdir (dir);
        if (!subdir.cd(subdirPath)) {
            continue;
        }
        if (subdir.exists(RULE_FILE)) {
            QString rule = subdir.absoluteFilePath(RULE_FILE);
            d->rules.append(rule);
            QUrl source = QUrl::fromLocalFile(rule);
            qDebug() << "Rule found:" << rule;
            if (d->engine->addComponent(QUrl::fromLocalFile(rule))) {
                d->loadingComponents.insert(source);
            }
        }
    }

    if (rules != d->rules) {
        emit rulesChanged();
    }
}

void EngineManager::stop()
{
    Q_D(EngineManager);
    if (d->running) {
        if (!d->loadingComponents.isEmpty()) {
            d->loadingComponents.clear();
        }

        d->engine->stop();
        d->running = false;
        emit runningChanged();
    }
}

bool EngineManager::IsRunning() const
{
    return isRunning();
}

QStringList EngineManager::Rules() const
{
    return rules();
}

void EngineManager::ReloadEngine()
{
    return reloadEngine();
}

void EngineManager::Stop()
{
    return stop();
}

bool EngineManager::AddRule(const QString &rule)
{
    return addRule(rule);
}

bool EngineManager::RemoveRule(const QString &path)
{
    return removeRule(path);
}

bool EngineManager::EditRule(const QString &path, const QString &rule)
{
    return editRule(path, rule);
}

#include "moc_enginemanager.cpp"
