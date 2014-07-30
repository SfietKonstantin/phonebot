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

#include "profileaction.h"
#include <action_p.h>
#include <Profile>
#include <choicemodel.h>

class ProfileActionPrivate: public ActionPrivate
{
public:
    explicit ProfileActionPrivate(Action *q);
    Profile *profileObject;
    QString profile;
};

ProfileActionPrivate::ProfileActionPrivate(Action *q)
    : ActionPrivate(q), profileObject(0)
{
}

ProfileAction::ProfileAction(QObject *parent) :
    Action(*(new ProfileActionPrivate(this)), parent)
{
    Q_D(ProfileAction);
    d->profileObject = new Profile(this);
}

QString ProfileAction::profile() const
{
    Q_D(const ProfileAction);
    return d->profile;
}

void ProfileAction::setProfile(const QString &profile)
{
    Q_D(ProfileAction);
    if (d->profile != profile) {
        d->profile = profile;
        emit profileChanged();
    }
}

bool ProfileAction::execute(Rule *rule)
{
    Q_D(ProfileAction);
    Q_UNUSED(rule);
    return d->profileObject->setActiveProfile(d->profile);
}

ProfileActionMeta::ProfileActionMeta(QObject *parent)
    : AbstractMetaData(parent)
{
}

QString ProfileActionMeta::name() const
{
    return tr("Profile");
}

QString ProfileActionMeta::description() const
{
    return tr("This action allows to change the profile when triggered.");
}

MetaProperty * ProfileActionMeta::getProperty(const QString &property, QObject *parent) const
{
    if (property == "profile") {
        ChoiceModel *choiceModel = new ChoiceModel(parent);
        choiceModel->addEntry(tr("Standard profile"), "ambience");
        choiceModel->addEntry(tr("Silent profile"), "silent");
        return MetaProperty::createChoice(property, tr("Profile to set"), choiceModel, parent);
    }
    return 0;
}
