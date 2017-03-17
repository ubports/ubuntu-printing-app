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
import QtQuick 2.4
import QtTest 1.1
import Ubuntu.Test 1.0
import "../../components"

Item {
    width: units.gu(100)
    height: units.gu(75)

    SelectorRow {
        id: selectorRow
        model: []
    }

    SignalSpy {
        id: expansionCompletedSpy
        signalName: "expansionCompleted"
        target: selectorRow
    }

    SignalSpy {
        id: selectedIndexSpy
        signalName: "onSelectedIndexChanged"
        target: selectorRow
    }

    SignalSpy {
        id: selectedValueSpy
        signalName: "onSelectedValueChanged"
        target: selectorRow
    }

    UbuntuTestCase {
        name: "SelectorRowTestCase"
        when: windowShown

        readonly property string dataLabelText: "Text"
        readonly property var dataModel: ["A", "B", "C"]
        readonly property int dataSelectedIndex: 0
        readonly property int dataSecondarySelectedIndex: 1

        function init() {
            selectorRow.model = dataModel;
            selectorRow.selectedIndex = dataSelectedIndex;
            selectorRow.text = dataLabelText;

            expansionCompletedSpy.clear();
            selectedIndexSpy.clear();
            selectedValueSpy.clear();

            waitForRendering(selectorRow);
        }

        function test_changeOption() {
            var selector = findChild(selectorRow, "selector");

            // Check selectedIndex is the starting index
            compare(selectorRow.selectedIndex, dataSelectedIndex);

            // Expand the selector
            mouseClick(selector);
            expansionCompletedSpy.wait();
            compare(expansionCompletedSpy.count, 1);

            // Click on the secondary index
            var option1 = findChild(selectorRow, "option" + dataSecondarySelectedIndex);
            mouseClick(option1);

            // Check that the selectedIndex is the secondaryIndex
            selectedIndexSpy.wait();
            compare(selectedIndexSpy.count, 1);
            compare(selectorRow.selectedIndex, dataSecondarySelectedIndex);
        }

        function test_label() {
            // Check that the label is correct
            var label = findChild(selectorRow, "label");
            compare(label.text, dataLabelText);
        }

        function test_model() {
            // Check the model length is the starting value
            compare(selectorRow.model.length, dataModel.length);

            // Remove the last entry from the model
            var tmpModel = dataModel;
            tmpModel.pop();
            selectorRow.model = tmpModel;

            // Check the model length changed
            compare(selectorRow.model.length, tmpModel.length);
        }

        function test_selectedIndex() {
            // Check selectedIndex is the starting index
            compare(selectorRow.selectedIndex, dataSelectedIndex);

            // Change the index to the secondaryIndex
            selectorRow.selectedIndex = dataSecondarySelectedIndex;

            // Check the index changed
            selectedIndexSpy.wait();
            compare(selectedIndexSpy.count, 1);
            compare(selectorRow.selectedIndex, dataSecondarySelectedIndex);
        }

        function test_selectedValue() {
            // Check that selectedValue is the starting value
            compare(selectorRow.selectedValue, dataModel[dataSelectedIndex]);

            // Change to the secondaryIndex
            selectorRow.selectedIndex = dataSecondarySelectedIndex;

            // Check that the selectedValue changes
            selectedValueSpy.wait();
            compare(selectorRow.selectedValue, dataModel[dataSecondarySelectedIndex]);
        }
    }
}
