import QtQuick 2.4
import QtQuick.Layouts 1.1

import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItems

RowLayout {
    implicitHeight: selector.implicitHeight
    Layout.maximumWidth: parent.width

    property alias model: selector.model
    property alias selectedIndex: selector.selectedIndex
    property alias text: label.text

    Label {
        id: label
        Layout.preferredWidth: units.gu(10)
    }

    ListItems.ItemSelector {
        id: selector
        containerHeight: itemHeight * 4
        Layout.fillWidth: true
        Layout.preferredWidth: units.gu(5)
//        Layout.minimumWidth: units.gu(20)
//        Layout.maximumWidth: units.gu(40)
        selectedIndex: 0
    }
}
