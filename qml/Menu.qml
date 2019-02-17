import QtQuick 2.0

Rectangle {
    id: gameMenu;

    anchors.fill: parent;

    color: "white";

    signal start(int numPlayers, var humanList);

    property int numPlayers: 8;
    property var humanList: [true, false, false, false, false, false, false, false];

    readonly property int maxPlayers: 8;

    Image {
        anchors.fill: parent;
        anchors.leftMargin: -100;
        anchors.rightMargin: -100;
        anchors.bottomMargin: -100;
        anchors.topMargin: -100;
        source: "qrc:///pixmaps/Background.png";
        opacity: 0.3;
        fillMode: Image.PreserveAspectCrop;
    }

    FontLoader {
        id: titleFont;
        source: "qrc:///fonts/unlearn2.ttf"
        onStatusChanged: console.log(status);
    }

    Text {
        id: txtTitle;

        anchors.top: parent.top;
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.topMargin: 100;

        height: 120;

        horizontalAlignment: Text.AlignHCenter;
        verticalAlignment: Text.AlignVCenter;

        text: "DICE WARS";
        font.bold: false;
        font.pointSize: 140;
        color: "black";

        font.family: titleFont.name;
    }

    Text {
        id: txtSubtitle;

        anchors.top: txtTitle.bottom;
        anchors.right: parent.right;
        anchors.topMargin: 5;
        anchors.rightMargin: 120;

        text: "Reloaded";

        linkColor: "black";

        font.pointSize: 30;
        font.bold: false;
        font.family: titleFont.name;
    }

    Text {
        id: txtCopyright;

        anchors.bottom: parent.bottom;
        anchors.left: parent.left;
        anchors.bottomMargin: 10;
        anchors.leftMargin: 10;

        text: "© Fernando Andreu 2015";

        linkColor: "black";

        font.pointSize: 16;
        font.bold: true;

        MouseArea {
            anchors.fill: parent;
            cursorShape: Qt.PointingHandCursor;

            onClicked: Qt.openUrlExternally("http://fernando.andreu.info");
        }
    }

    Rectangle {
        id: btnStart;

        anchors.right: parent.right;
        anchors.bottom: parent.bottom;
        anchors.rightMargin: 20;
        anchors.bottomMargin: 20;

        property color textColor: "black";


        ColorAnimation on color {
            duration: 200;
        }

        width: 200;
        height: 60;
        border.color: "black";
        border.width: 3;
        radius: 10;

        Text {
            anchors.centerIn: parent;
            font.pointSize: 20;
            font.bold: true;
            text: "Start!";
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
                parent.color = "white";
                parent.textColor = "black";
            }

            onClicked: {
                for (var i = 0; i < gameMenu.maxPlayers; i++)
                    gameMenu.humanList[i] = playerOptions.itemAt(i).human;
                gameMenu.start(gameMenu.numPlayers, gameMenu.humanList);
            }
        }
    }

    Rectangle {
        id: btnRemovePlayer;

        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.horizontalCenterOffset: -40;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: 50;

        property color textColor: "black";

        width: 60;
        height: 60;
        border.color: "black";
        border.width: 3;
        radius: 30;

        Text {
            anchors.centerIn: parent;
            font.pointSize: 24;
            font.bold: true;
            text: "-";
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
                parent.color = "white";
                parent.textColor = "black";
            }

            onClicked: {
                if (gameMenu.numPlayers < 3) return;
                gameMenu.numPlayers--;
                playerOptions.itemAt(gameMenu.numPlayers).visible = false;
            }
        }
    }

    Rectangle {
        id: btnAddPlayer;

        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.horizontalCenterOffset: 40;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: 50;

        property color textColor: "black";

        width: 60;
        height: 60;
        border.color: "black";
        border.width: 3;
        radius: 30;

        Text {
            anchors.centerIn: parent;
            font.pointSize: 24;
            font.bold: true;
            text: "+";
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
                parent.color = "white";
                parent.textColor = "black";
            }

            onClicked: {
                if (gameMenu.numPlayers >= gameMenu.maxPlayers) return;
                gameMenu.numPlayers++;
                playerOptions.itemAt(gameMenu.numPlayers - 1).visible = true;
            }
        }
    }

    Grid {
        columns: 4;
        spacing: 50;

        anchors.fill: parent;
        anchors.leftMargin: 100;
        anchors.rightMargin: 100;
        anchors.topMargin: 400;

        Component.onCompleted: {
            for (var i = 0; i < gameMenu.maxPlayers; i++)
            {
                if (i >= gameMenu.numPlayers) playerOptions.itemAt(i).visible = false;
                playerOptions.itemAt(i).human = gameMenu.humanList[i];
            }
        }

        Repeater {
            id: playerOptions;
            model: gameMenu.maxPlayers;

            Item {
                width: 200;
                height: 80;

                property bool human: false;

                Rectangle {
                    id: playerNumber;

                    height: 40;
                    anchors.verticalCenter: parent.verticalCenter;
                    anchors.left: parent.left;
                    width: 40;

                    border.color: "black";
                    border.width: 3;

                    radius: 20;

                    Text {
                        anchors.fill: parent;

                        horizontalAlignment: Text.AlignHCenter;
                        verticalAlignment: Text.AlignVCenter;

                        text: index + 1;

                        font.pointSize: 20;
                    }
                }


                Image {
                    id: imgPlayer;

                    anchors.verticalCenter: parent.verticalCenter;
                    anchors.left: playerNumber.right;
                    anchors.leftMargin: 10;

                    width: 70;
                    height: 70;

                    source: "qrc:///pixmaps/Player" + index + "_Dice" + (index%6+1) + ".png";
                }

                Rectangle {
                    id: btnHuman;

                    anchors.left: imgPlayer.right;
                    anchors.top: parent.top;
                    anchors.topMargin: 5;
                    anchors.leftMargin: 10;

                    property bool mouseOver: false;

                    width: 70;
                    height: 30;
                    border.color: "black";
                    border.width: 2;
                    radius: 5;
                    color: mouseOver || parent.human ? "black" : "white";

                    Text {
                        anchors.centerIn: parent;
                        font.pointSize: 12;
                        font.bold: true;
                        text: "Human";
                        color: parent.mouseOver || parent.parent.human ? "white" : "black";
                    }

                    MouseArea {
                        anchors.fill: parent;
                        hoverEnabled: true;
                        cursorShape: Qt.PointingHandCursor;

                        onEntered: {
                            parent.mouseOver = true;
                        }

                        onExited: {
                            parent.mouseOver = false;
                        }

                        onClicked: {
                            parent.parent.human = true;
                        }
                    }
                }

                Rectangle {
                    id: btnCPU;

                    anchors.left: imgPlayer.right;
                    anchors.bottom: parent.bottom;
                    anchors.bottomMargin: 5;
                    anchors.leftMargin: 10;

                    property bool mouseOver: false;

                    width: 70;
                    height: 30;
                    border.color: "black";
                    border.width: 2;
                    radius: 5;
                    color: mouseOver || !parent.human ? "black" : "white";

                    Text {
                        anchors.centerIn: parent;
                        font.pointSize: 12;
                        font.bold: true;
                        text: "CPU";
                        color: parent.mouseOver || !parent.parent.human ? "white" : "black";
                    }

                    MouseArea {
                        anchors.fill: parent;
                        hoverEnabled: true;
                        cursorShape: Qt.PointingHandCursor;

                        onEntered: {
                            parent.mouseOver = true;
                        }

                        onExited: {
                            parent.mouseOver = false;
                        }

                        onClicked: {
                            parent.parent.human = false;
                        }
                    }
                }
            }
        }

    }
}

