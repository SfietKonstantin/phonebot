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

.pragma library
.import harbour.phonebot 1.0 as PhoneBot

function getSource(type, subType) {
    switch (type) {
    case PhoneBot.MetaProperty.String:
        switch (subType) {
        case PhoneBot.MetaProperty.ChoiceSubType:
            return Qt.resolvedUrl("ComboBox.qml")
        case PhoneBot.MetaProperty.ApplicationSubType:
            return Qt.resolvedUrl("DialogButton.qml")
        default:
            return Qt.resolvedUrl("TextField.qml")
        }
    case PhoneBot.MetaProperty.Int:
        return Qt.resolvedUrl("IntField.qml")
    case PhoneBot.MetaProperty.Double:
        return Qt.resolvedUrl("DoubleField.qml")
    case PhoneBot.MetaProperty.Bool:
        return Qt.resolvedUrl("Switch.qml")
    default:
        return Qt.resolvedUrl("DialogButton.qml")
    }
}

function openSelector(type, subType, stack, component, index) {
    switch (type) {
    case PhoneBot.MetaProperty.Time:
        stack.push(Qt.resolvedUrl("TimePicker.qml"), {"component": component, "index": index})
        return true
    case PhoneBot.MetaProperty.String:
        switch (subType) {
        case PhoneBot.MetaProperty.ApplicationSubType:
            stack.push(Qt.resolvedUrl("ApplicationPicker.qml"), {"component": component, "index": index})
            return true
        default:
            return false
        }
    default:
        return false
    }
}
