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
#include <QtCore/QStandardPaths>

static const char *RULE_FILE = "rule.qml";

EngineManager::EngineManager(QObject *parent)
    : QObject(parent)
    , m_running(false)
    , m_engine(new PhoneBotEngine(this))
{
    m_engine->registerTypes();
    connect(m_engine, &PhoneBotEngine::componentLoadingFinished,
            this, &EngineManager::slotComponentLoadingFinished);
}

EngineManager::~EngineManager()
{
    stop();
}

void EngineManager::reloadEngine()
{
    stop();

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
            QUrl source = QUrl::fromLocalFile(rule);
            qDebug() << "Rule found:" << rule;
            if (m_engine->addComponent(QUrl::fromLocalFile(rule))) {
                m_loadingComponents.insert(source);
            }
        }
    }
}

void EngineManager::stop()
{
    if (!m_loadingComponents.isEmpty()) {
        m_loadingComponents.clear();
    }

    if (m_running) {
        m_engine->stop();
        m_running = false;
    }
}

void EngineManager::slotComponentLoadingFinished(const QUrl &url, bool ok)
{
    Q_UNUSED(ok)

    // If it's empty, it means that stop has been called
    // Don't try to launch engine
    if (m_loadingComponents.isEmpty()) {
        return;
    }

    m_loadingComponents.remove(url);
    if (m_loadingComponents.isEmpty()) {
        m_engine->start();
        m_running = true;
    }
}
