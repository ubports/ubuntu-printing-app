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
import "../../ubuntu-printing-app/components"

import Ubuntu_Printing_App 1.0
import Ubuntu.Settings.Printers 0.1

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
            mouseClick(selectorRow);

            expansionCompletedSpy.wait();
            compare(expansionCompletedSpy.count, 1);

            var option1 = findChild(selectorRow, "option1");
            mouseClick(option1);

            selectedIndexSpy.wait();
            compare(selectedIndexSpy.count, 1);
            compare(selectorRow.selectedIndex, 1);
        }

        function test_label() {
            var label = findChild(selectorRow, "label");

            compare(label.text, dataLabelText);
        }

        function test_model() {
            compare(selectorRow.model.length, dataModel.length);

            var tmpModel = dataModel;
            tmpModel.pop();

            selectorRow.model = tmpModel;

            compare(selectorRow.model.length, tmpModel.length);
        }

        function test_selectedIndex() {
            compare(selectorRow.selectedIndex, dataSelectedIndex);

            selectorRow.selectedIndex = dataSecondarySelectedIndex;

            selectedIndexSpy.wait();
            compare(selectedIndexSpy.count, 1);
            compare(selectorRow.selectedIndex, dataSecondarySelectedIndex);
        }

        function test_selectedValue() {
            compare(selectorRow.selectedValue, dataModel[dataSelectedIndex]);

            selectorRow.selectedIndex = dataSecondarySelectedIndex;

            selectedValueSpy.wait();
            compare(selectorRow.selectedValue, dataModel[dataSecondarySelectedIndex]);
        }
    }
}
