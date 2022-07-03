/*
 * Copyright 2017 Canonical Ltd.
 *
 * This file is part of ubuntu-printing-app.
 *
 * ubuntu-printing-app is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * ubuntu-printing-app is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored-by: Andrew Hayzen <andrew.hayzen@canonical.com>
 */
import QtQuick 2.12
import QtTest 1.1
import Ubuntu.Test 1.0
import "../../components"

Item {
    width: units.gu(100)
    height: units.gu(75)

    TextFieldRow {
        id: textFieldRow
    }

    IntValidator {
        id: intValidator
        bottom: 1
        top: 9
    }

    SignalSpy {
        id: acceptableInputSpy
        signalName: "onAcceptableInputChanged"
        target: textFieldRow
    }

    SignalSpy {
        id: valueSpy
        signalName: "onValueChanged"
        target: textFieldRow
    }

    UbuntuTestCase {
        name: "TextFieldRowTestCase"
        when: windowShown

        readonly property bool dataEnabled: true
        readonly property string dataPlaceholderText: "placeholder"

        function init() {
            textFieldRow.enabled = dataEnabled;
            textFieldRow.inputMethodHints = Qt.ImhNone;
            textFieldRow.placeholderText = dataPlaceholderText;
            textFieldRow.text = "test";
            textFieldRow.validator = null;
            textFieldRow.value = "";

            acceptableInputSpy.clear();
            valueSpy.clear();

            waitForRendering(textFieldRow);
        }

        function test_acceptableInput() {
            var textField = findChild(textFieldRow, "textField");

            // Set the validator to int (1-9)
            textFieldRow.validator = intValidator;

            // Set the value to an invalid value
            textFieldRow.value = "99";

            valueSpy.wait();
            acceptableInputSpy.wait();

            compare(valueSpy.count, 1);
            compare(acceptableInputSpy.count, 1);

            // Check the value was not acceptable
            compare(textFieldRow.acceptableInput, false);
            compare(textField.acceptableInput, false);

            // Set the value to a valid value
            textFieldRow.value = "2";

            valueSpy.wait();
            acceptableInputSpy.wait();

            compare(valueSpy.count, 2);
            compare(acceptableInputSpy.count, 2);

            // Check the value was acceptable
            compare(textFieldRow.acceptableInput, true);
            compare(textField.acceptableInput, true);
        }

        function test_enabled() {
            // Check textField enabled state is starting value
            var textField = findChild(textFieldRow, "textField");
            compare(textFieldRow.enabled, dataEnabled);

            // Flip the enabled state
            textFieldRow.enabled = !dataEnabled;

            // Check that the enabled state changes
            tryCompare(textField, "enabled", !dataEnabled);
        }

        function test_inputMethodHints() {
            // Check that inputMethodHints is starting value
            var textField = findChild(textFieldRow, "textField");
            compare(textFieldRow.inputMethodHints, Qt.ImhNone);
            compare(textField.inputMethodHints, Qt.ImhNone);

            // Change the inputMethodHints mode
            textFieldRow.inputMethodHints = Qt.ImhDigitsOnly;

            // Check it changes in the textField
            compare(textField.inputMethodHints, Qt.ImhDigitsOnly);
        }

        function test_placeholderText() {
            // Check the placeholder text is correct
            var textField = findChild(textFieldRow, "textField");
            compare(textField.placeholderText, dataPlaceholderText);
            compare(textFieldRow.placeholderText, dataPlaceholderText);

            // Check that the value is not the placeholder
            verify(textField.text !== dataPlaceholderText);
            verify(textFieldRow.value !== dataPlaceholderText);
        }

        function test_text() {
            // Check that the text is the correct value
            var label = findChild(textFieldRow, "label");
            compare(label.text, textFieldRow.text);
        }

        function test_validator() {
            // Check that a validator can be set
            var textField = findChild(textFieldRow, "textField");
            textFieldRow.validator = intValidator;

            compare(textField.validator, intValidator);
        }

        function test_value() {
            // Check that the value is the starting value
            var textField = findChild(textFieldRow, "textField");
            compare(textFieldRow.value, "");
            compare(textField.text, "");

            // Click on the text field
            mouseClick(textField);

            // Enter "a"
            keyPress(Qt.Key_A);

            // Check that "a" is set to the value
            valueSpy.wait();
            compare(valueSpy.count, 1);
            compare(textField.text, "a");
            compare(textFieldRow.value, "a");
        }
    }
}
