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

#ifndef ENGINEMANAGER_H
#define ENGINEMANAGER_H

#include <QtCore/QObject>
#include <QtCore/QSet>
#include <phonebotengine.h>

class EngineManagerPrivate;
class EngineManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool running READ isRunning NOTIFY runningChanged)
    Q_PROPERTY(QStringList rules READ rules NOTIFY rulesChanged)
public:
    explicit EngineManager(QObject *parent = 0);
    virtual ~EngineManager();
    bool isRunning() const;
    QStringList rules() const;
    bool addRule(const QString &rule);
    bool removeRule(const QString &path);
    bool editRule(const QString &path, const QString &rule);
public Q_SLOTS:
    void reloadEngine();
    void stop();
Q_SIGNALS:
    void runningChanged();
    void rulesChanged();
public Q_SLOTS: // For DBus
    bool IsRunning() const;
    QStringList Rules() const;
    void ReloadEngine();
    void Stop();
    bool AddRule(const QString &rule);
    bool RemoveRule(const QString &path);
    bool EditRule(const QString &path, const QString &rule);
protected:
    QScopedPointer<EngineManagerPrivate> d_ptr;
private:
    Q_PRIVATE_SLOT(d_func(), void slotComponentLoadingFinished(const QUrl &url, bool ok))
    Q_DECLARE_PRIVATE(EngineManager)
};

#endif // ENGINEMANAGER_H
