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
import "typesolver.js" as TypeSolver

Dialog {
    id: container
    property QtObject rule
    property QtObject component
    property int type
    property int index: -1
    acceptDestinationAction: PageStackAction.Pop
    onAccepted: rule.saveComponent(container.index)
    onRejected: rule.discardComponent(container.index)
    Component.onCompleted: {
        container.component = rule.createTempClonedComponent(container.type, container.index)
    }

    SilicaFlickable {
        id: flickable
        function setType(component) {
            container.component = rule.createTempComponent(container.type, container.index, component)
        }

        anchors.fill: parent

        Column {
            width: parent.width
            DialogHeader {
                acceptText: "Save"
            }
            RuleButton {
                text: container.component ? PhoneBot.componentName(container.type)
                                          : PhoneBot.selectComponent(container.type)
                secondaryText: container.component ? container.component.name : ""
                onClicked: {
                    var page = pageStack.push(Qt.resolvedUrl("ComponentsDialog.qml"),
                                              {acceptDestination: container,
                                               component: container.component ? container.component.type : "",
                                               type: container.type})
                    page.componentSelected.connect(flickable.setType)
                }
            }
            SectionHeader {
                visible: container.component != null && container.component.count > 0
                text: "Properties"
            }
            Repeater {
                model: container.component
                TypeField {
                    type: model.type
                    value: model.value
                    Component.onCompleted: console.debug(">>> " + model.value)
                    component: container.component
                    index: model.index
                    stack: pageStack
                }
            }
        }
    }
}
