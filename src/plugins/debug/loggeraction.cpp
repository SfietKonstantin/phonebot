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

#include "loggeraction.h"
#include <QtCore/QDebug>
#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QMetaObject>
#include <QtCore/QMetaProperty>
#include <QtCore/QStandardPaths>
#include <QtQml/QQmlListReference>
#include <condition.h>
#include <rule.h>
#include <trigger.h>

static void dumpMetadata(QTextStream &stream, QObject *object, int indent)
{
    const QMetaObject *meta = object->metaObject();
    stream << QString("    ").repeated(indent) << "class: " << meta->className() << endl;
    for (int i = 0; i < meta->propertyCount(); ++i) {
        const QMetaProperty &property = meta->property(i);
        QVariant value = property.read(object);
        stream << QString("    ").repeated(indent) <<  property.name() << ": \"" << value.toString()
               << "\" (" << value.typeName() << ")" << endl;
    }
}

LoggerAction::LoggerAction(QObject *parent) :
    Action(parent)
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    qDebug() << "Writing logs at" << path;
    QDir::root().mkpath(path);
}

bool LoggerAction::execute(Rule *rule)
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    QDir dir (path);
    QFile log (dir.absoluteFilePath("phonebot.log"));
    if (!log.open(QIODevice::WriteOnly | QIODevice::Append)) {
        return false;
    }

    QTextStream stream (&log);
    stream << "Rule {" << endl;
    stream << "    name: \"" << rule->name() << "\"" << endl;
    stream << "    time: " << QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss") << endl;

    if (rule->trigger()) {
        stream << "    trigger: Trigger {" << endl;
        dumpMetadata(stream, rule->trigger(), 2);
        stream << "    }" << endl;
    } else {
        stream << "    trigger: NULL" << endl;
    }

    if (rule->condition()) {
        stream << "    condition: Condition {" << endl;
        dumpMetadata(stream, rule->condition(), 2);
        stream << "    }" << endl;
    } else {
        stream << "    condition: NULL" << endl;
    }

    QQmlListReference actions (rule, "actions");
    stream << "    actions: [" << endl;
    for (int i = 0; i < actions.count(); ++i) {
        stream << "        Action {" << endl;
        dumpMetadata(stream, actions.at(i), 3);
        stream << "        }" << endl;
    }
    stream << "    }" << endl;
    stream << "}" << endl << endl;

    log.close();
    return true;
}
