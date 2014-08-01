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

import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.phonebot 1.0

Dialog {
    id: container
    property bool add: true
    property int index: -1
    property QtObject model
    property QtObject rule
    onAccepted: model.pushRule(index, rule)
    onRejected: model.discardRule(rule)

    SilicaFlickable {
        anchors.fill: parent

        Column {
            width: parent.width
            DialogHeader {
                acceptText: container.add ? "Add rule" : "Edit rule"
            }

            TextField {
                width: parent.width
                placeholderText: "Name"
                label: "Name"
                text: rule != null ? rule.name : ""
                onTextChanged: rule.name = text
            }

            SectionHeader {
                text: qsTr("Conditions")
            }

            RuleButton {
                text: qsTr("Trigger")
                secondaryText: container.rule != null ? (container.rule.trigger != null ? container.rule.trigger.name : "") : ""
                enabled: container.rule != null
                onClicked: pageStack.push(Qt.resolvedUrl("ComponentConfigDialog.qml"),
                                          {acceptDestination: container,
                                           rule: container.rule,
                                           type: PhoneBot.Trigger})
            }
            RuleButton {
                text: qsTr("Condition")
                secondaryText: container.rule != null ? (container.rule.condition != null ? container.rule.condition.name : "") : ""
                enabled: container.rule != null
                onClicked: pageStack.push(Qt.resolvedUrl("ComponentConfigDialog.qml"),
                                          {acceptDestination: container,
                                           rule: container.rule,
                                           type: PhoneBot.Condition})
            }

            SectionHeader {
                text: qsTr("Actions")
            }

            Repeater {
                model: container.rule.actions
                RuleButton {
                    id: action
                    function removeAction() {
                        container.rule.actions.remove(model.index)
                        animateRemoval(action)
                    }
                    text: qsTr("Action %n", "", index + 1)
                    secondaryText: model.component != null ? model.component.name : ""
                    onClicked: pageStack.push(Qt.resolvedUrl("ComponentConfigDialog.qml"),
                                              {acceptDestination: container,
                                               rule: container.rule.actions,
                                               component: model.component,
                                               type: PhoneBot.Action,
                                               index: model.index})
                    onPressAndHold: {
                        action.showMenu()
                    }
                    menu: ContextMenu {
                        MenuItem {
                            text: qsTr("Remove action")
                            onClicked: action.remorseAction(qsTr("Removing action"), removeAction)
                        }
                    }
                }
            }
            RuleButton {
                text: qsTr("Add action")
                onClicked: pageStack.push(Qt.resolvedUrl("ComponentConfigDialog.qml"),
                                          {acceptDestination: container,
                                           rule: container.rule.actions,
                                           type: PhoneBot.Action,
                                           index: container.rule.actions.count})
            }
        }

        VerticalScrollDecorator {}
    }
}
