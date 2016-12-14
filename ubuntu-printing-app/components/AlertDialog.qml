import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3

//Component {
    Dialog {
        id: dialog

        Button {
            text: i18n.tr("OK")

            onClicked: PopupUtils.close(dialog)
        }
    }
//}
