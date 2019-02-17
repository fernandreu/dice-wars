import QtQuick 2.4

import QtQuick.Window 2.0

Window {
    id: mainWindow;

    visible: true
    width: 1130;
    height: 800;
    minimumWidth: width;
    minimumHeight: height;
    maximumWidth: width;
    maximumHeight: height;

    flags: Qt.Dialog | Qt.CustomizeWindowHint | Qt.WindowTitleHint | Qt.WindowMinimizeButtonHint | Qt.WindowCloseButtonHint;

    property int numPlayers : 8;
    property var humanList: [true, false, false, false, false, false, false, false];

    Loader {
        id: contentLoader;

        anchors.fill: parent;
        source: "qrc:/Menu.qml";
    }

    Connections {
        id: connGame;

        target: contentLoader.item;
        ignoreUnknownSignals: true;

        onStart: {
            mainWindow.numPlayers = numPlayers;
            mainWindow.humanList = humanList;
            contentLoader.setSource("qrc:/Game.qml", { "numPlayers": numPlayers, "humanList": humanList});
        }

        onRestart: {
            gameOverScreen.visible = false;
        }

        onReturnToMenu: {
            gameOverScreen.visible = false;
            contentLoader.setSource("qrc:/Menu.qml", { "numPlayers": mainWindow.numPlayers, "humanList": mainWindow.humanList });
        }

        onVictory: {
            gameOverScreen.message = "PLAYER " + (player+1) + " WON!";
            gameOverScreen.player = player;
            gameOverScreen.visible = true;
        }
    }

    Rectangle {
        id: gameOverScreen;

        visible: false;

        color: "#aaffffff";
        border.color: "black";
        border.width: 5;
        radius: 20;

        anchors.centerIn: parent;
        width: 420;
        height: 280;

        property string message: "";
        property int player: -1;

        Text {
            anchors.left: parent.left;
            anchors.right: parent.right;
            anchors.bottom: parent.bottom;
            height: 160;

            color: "black";
            font.pointSize: 35;
            font.bold: true;

            text: parent.message;

            horizontalAlignment: Text.AlignHCenter;
            verticalAlignment: Text.AlignVCenter;
        }

        Image {
            anchors.horizontalCenter: parent.horizontalCenter;
            anchors.top: parent.top;
            anchors.topMargin: 40;

            width: 120;
            height: 120;

            source: parent.player >= 0 ? "qrc:///pic/Player" + parent.player + "_Dice" + (parent.player%6+1) + ".png" : "";
        }

        Rectangle {
            id: btnMainMenu;

            anchors.right: parent.right;
            anchors.bottom: parent.bottom;
            anchors.rightMargin: 10;
            anchors.bottomMargin: 10;

            property color textColor: "black";
            color: "transparent";

            width: 120;
            height: 30;
            border.color: "black";
            border.width: 3;
            radius: 10;

            Text {
                anchors.centerIn: parent;
                font.pointSize: 12;
                font.bold: true;
                text: "Main Menu";
                color: parent.textColor;
            }

            MouseArea {
                anchors.fill: parent;
                hoverEnabled: true;
                cursorShape: Qt.PointingHandCursor;


                onEntered: {
                    parent.color = "black";
                    parent.textColor = "white";
                }

                onExited: {
                    parent.color = "transparent";
                    parent.textColor = "black";
                }

                onClicked: {
                    gameOverScreen.visible = false;
                    contentLoader.setSource("qrc:/Menu.qml", { "numPlayers": mainWindow.numPlayers, "humanList": mainWindow.humanList });
                }
            }
        }
    }
}
