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

#ifndef RULESMODEL_H
#define RULESMODEL_H

#include <QtCore/QAbstractListModel>
#include "ruledefinition.h"

class RulesModelData;
class RulesModel : public QAbstractListModel
{
   Q_OBJECT
   Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
   enum Roles {
       NameRole,
       DefinitionRole
   };
   explicit RulesModel(QObject *parent = 0);
   virtual ~RulesModel();
   QHash<int, QByteArray> roleNames() const;
   int rowCount(const QModelIndex &parent = QModelIndex()) const;
   QVariant data(const QModelIndex &index, int role) const;
   int count() const;
   Q_INVOKABLE RuleDefinition * createRule();
signals:
   void countChanged();
public slots:
   void reload();
   bool pushRule(RuleDefinition *rule);
   void discardRule(RuleDefinition *rule) const;
private:
    QList<RulesModelData *> m_data;
};

#endif // RULESMODEL_H
