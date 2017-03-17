/*
 * Copyright 2016 Canonical Ltd.
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
import Ubuntu.Components 1.3
import Ubuntu.Content 1.3

Page {
    id: picker
    header: PageHeader {
        leadingActionBar {
            actions: [
                Action {
                    iconName: "close"

                    onTriggered: {
                        if (picker.curTransfer) {
                            picker.curTransfer.state = ContentTransfer.Aborted;
                        }

                        picker.pageStack.pop()
                    }
                }
            ]
        }
        title: i18n.tr("Open With...")
    }

    property var curTransfer: null
    property var url

    Component {
        id: contentItem
        ContentItem {}
    }

    function __exportItems(url) {
        console.debug("Export Url", url)

        function __export(item) {
            picker.curTransfer.items = [item];

            // FIXME: setting state to Charged can cause UI freeze for few seconds
            picker.curTransfer.state = ContentTransfer.Charged;
        }

        if (picker.curTransfer.state === ContentTransfer.InProgress) {
            var incubator = contentItem.incubateObject(picker, {"url": url});

            if (incubator.status !== Component.Ready) {
                incubator.onStatusChanged = function(status) {
                    if (status === Component.Ready) {
                        __export(incubator.object);
                    }
                }
            } else {
                __export(incubator.object);
            }
        }
    }

    ContentPeerPicker {
        id: peerPicker
        anchors {
            bottom: parent.bottom
            fill: undefined
            left: parent.left
            right: parent.right
            top: picker.header.bottom
        }
        contentType: ContentType.Documents
        customPeerModelLoader: Loader {
            asynchronous: true
            sourceComponent: ContentPeerModel {
                contentType: ContentType.Documents
                handler: ContentHandler.Destination

                onPeersChanged: {
                    var tmpPeers = [];

                    for (var i=0; i < peers.length; i++) {
                        if (peers[i].appId !== "ubuntu-printing-app") {
                            tmpPeers.push(peers[i]);
                        }
                    }

                    peers = tmpPeers;
                }
            }
        }
        handler: ContentHandler.Destination
        showTitle: false
        visible: parent.visible

        onPeerSelected: {
            picker.curTransfer = peer.request();

            if (picker.curTransfer.state === ContentTransfer.InProgress) {
                picker.__exportItems(picker.url);
            } else {
                console.debug("Expected inProgress state, not exporting item")
            }
        }
    }

    ActivityIndicator {
        anchors {
            centerIn: parent
        }
        running: picker.curTransfer ? picker.curTransfer.state === ContentTransfer.InProgress || picker.curTransfer === ContentTransfer.Charged : false
    }

    Connections {
        target: picker.curTransfer
        onStateChanged: {
            if (picker.curTransfer.state === ContentTransfer.Collected) {
                Qt.quit();
            }
        }
    }
}
