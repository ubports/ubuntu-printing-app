import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Content 0.1
import Destination_App 1.0
/*!
    \brief MainView with a Label and Button elements.
*/

MainView {
    // objectName for functional testing purposes (autopilot-qt5)
    objectName: "mainView"

    // Note! applicationName needs to match the "name" field of the click manifest
    applicationName: "destination-app.ahayzen"

    width: units.gu(100)
    height: units.gu(75)

    Connections {
        target: ContentHub

        onImportRequested: {
            console.debug("State:", transfer.state)
            console.debug("Transfer:", transfer)
            console.debug("Items:", transfer.items)

            for (var i=0; i < transfer.items.length; i++) {
                var item = transfer.items[i];

                myType.url = item.url;

                console.debug("Item URL:", item.url);
            }
        }
    }

    MyType {
        id: myType
    }

    Page {
        header: PageHeader {
            id: pageHeader
            title: i18n.tr("destination-app")
            StyleHints {
                foregroundColor: UbuntuColors.orange
                backgroundColor: UbuntuColors.porcelain
                dividerColor: UbuntuColors.slate
            }
        }

        Label {
            id: label
            objectName: "label"
            anchors {
                horizontalCenter: parent.horizontalCenter
                top: pageHeader.bottom
                topMargin: units.gu(2)
            }
            text: "URL to print: " + myType.url
        }

        Button {
            objectName: "button"
            anchors {
                horizontalCenter: parent.horizontalCenter
                top: label.bottom
                topMargin: units.gu(2)
            }
            width: parent.width

            text: i18n.tr("Print!")

            onClicked: {
                if (myType.url === "") {
                    myType.url = Qt.resolvedUrl("/home/andy/Workspace/Work/Canonical/dump/2016-11-17T12:10:24");
                }

                console.debug("URL", myType.url, myType.url === "")

                myType.printType();
            }
        }
    }
}


