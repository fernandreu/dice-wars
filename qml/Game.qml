import QtQuick.Controls 1.2

import Hex 1.0

import QtQuick 2.4

Rectangle {
    id: gameContents;

    anchors.fill: parent;

    signal returnToMenu;
    signal restart;
    signal victory(int player, bool human);

    property int numPlayers: 8;
    property var humanList: [true, false, false, false, false, false, false, false];

    color: "white";

    /// The board of the game itself
    HexGrid {
        id: hexGrid;

        anchors.fill: parent;
        anchors.topMargin: 50;
        anchors.leftMargin: 50;

        gridWidth: 60;
        gridHeight: 40;
        radius: 10;

        numPlayers: parent.numPlayers;
        humanList: parent.humanList;

        numTerritories: 80;
        territorySize: 25;

        Component.onCompleted: {
            restartGame();
        }

        onShowAttackResult: {
            statusMessage.text = attack + " vs " + defense + ((attack > defense) ? " - VICTORY!" : " - Defeat...");
        }

        onConnTerrChanged: {
            if (connTerr == 0) {
                playerLabels.itemAt(player).visible = false;
            } else {
                playerLabels.itemAt(player).visible = true;
                playerLabels.itemAt(player).caption = connTerr;
            }
        }

        onPlayerTurnChanged: {
            var index = playerTurn - 1;
            if (index < 0) index = numPlayers - 1;
            // Disable all player lables, activating only the one for the new player
            for (var i = 0; i < parent.numPlayers; i++)
                playerLabels.itemAt(i).color = "transparent";
            playerLabels.itemAt(playerTurn).color = "yellow";
            // Toggle buttons / hints depending on whether the new player is human
            if (isPlayerHuman(playerTurn)) {
                btnEndTurn.visible = true;
                btnAutoMode.visible = true;
                statusMessage.text = "Your turn!";
            } else {
                btnEndTurn.visible = false;
                btnAutoMode.visible = false;
                statusMessage.text = "";
            }
        }

        onVictory: {
            gameContents.victory(player, human);
        }

        function restartGame() {
            initializeGrid();
            for (var i = 0; i < hexGrid.numPlayers; i++)
                playerLabels.itemAt(i).human = hexGrid.isPlayerHuman(i);
        }
    }

    /// Simple MouseArea covering the game board to detect all clicks and pass
    /// them through the board itself
    MouseArea {
        anchors.fill: hexGrid;

        onClicked: {
            hexGrid.processClick(mouse.x, mouse.y);
        }
    }

    /// Text block containing useful tips and messages depending on the game status
    Text {
        id: statusMessage;

        anchors.bottom: parent.bottom;
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.leftMargin: 50;
        height: 50;

        text: "";

        horizontalAlignment: Text.AlignHCenter;
        verticalAlignment: Text.AlignVCenter;
        font.pointSize: 24;
    }

    /// Portion of the bottom of the screen which will hold the scores of each player
    Row {
        anchors.bottom: statusMessage.top;
        anchors.bottomMargin: 0;
        anchors.horizontalCenter: parent.horizontalCenter;

        height: 50;

        Repeater {
            id: playerLabels;
            model: hexGrid.numPlayers;

            Rectangle {
                property string caption: "";
                property bool human: false;

                width: 120;
                height: 50;
                anchors.margins: 10;

                radius: 10;

                Rectangle {
                    anchors.verticalCenter: parent.verticalCenter;
                    anchors.leftMargin: 5;

                    width: 40;
                    height: 40;
                    radius: 15;

                    color: human ? "#55000000" : "transparent";
                }

                Image {
                    id: playerPic;
                    anchors.verticalCenter: parent.verticalCenter;
                    anchors.leftMargin: 10;

                    width: 40;
                    height: 40;

                    source: "qrc:///pixmaps/Player" + index + "_Dice" + (index%6+1) + ".png";
                }

                Text {
                    height: 40;
                    anchors.verticalCenter: parent.verticalCenter;
                    anchors.left: playerPic.right;
                    anchors.leftMargin: 10;
                    anchors.right: parent.right;

                    text: parent.caption;

                    font.pointSize: 24;
                }
            }
        }
    }

    /// Button to go back to the main menu
    Rectangle {
        id: btnMainMenu;

        anchors.left: parent.left;
        anchors.bottom: parent.bottom;
        anchors.leftMargin: 10;
        anchors.bottomMargin: 10;

        property color textColor: "black";

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
                gameContents.returnToMenu();
            }
        }
    }

    /// Button to restart the game without going through the main menu
    Rectangle {
        id: btnRestart;

        anchors.bottom: parent.bottom;
        anchors.left: btnMainMenu.right;
        anchors.bottomMargin: 10;
        anchors.leftMargin: 10;

        property color textColor: "black";

        width: 100;
        height: 30;
        border.color: "black";
        border.width: 3;
        radius: 10;

        Text {
            anchors.centerIn: parent;
            font.pointSize: 12;
            font.bold: true;
            text: "Restart";
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
                playerLabels.itemAt(hexGrid.playerTurn).color = "transparent";
                gameContents.restart();
                hexGrid.restartGame();
            }
        }
    }

    /// Button to finish a human turn once they cannot / do not want to perform
    /// any more actions
    Rectangle {
        id: btnEndTurn;

        anchors.right: parent.right;
        anchors.bottom: parent.bottom;
        anchors.rightMargin: 10;
        anchors.bottomMargin: 10;

        property color textColor: "black";

        width: 120;
        height: 40;
        border.color: "black";
        border.width: 3;
        radius: 10;

        Text {
            anchors.centerIn: parent;
            font.pointSize: 14;
            font.bold: true;
            text: "End Turn";
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
                hexGrid.endTurn();
            }
        }
    }

    /// Button that, when toggled, causes human turns to be played automatically
    Rectangle {
        id: btnAutoMode;

        anchors.right: btnEndTurn.left;
        anchors.bottom: parent.bottom;
        anchors.rightMargin: 10;
        anchors.bottomMargin: 10;

        property color textColor: "black";

        width: 100;
        height: 40;
        border.color: "black";
        border.width: 3;
        radius: 10;

        Text {
            anchors.centerIn: parent;
            font.pointSize: 14;
            font.bold: true;
            text: "Auto";
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
                btnAutoMode.visible = false;
                btnEndTurn.visible = false;
                hexGrid.startAITurn();
            }
        }
    }

    /// Hidden button at the top of the screen which, if toggled, will give
    /// human players much more probabilities of winning in territory battles
    Rectangle {
        id: btnCheat;

        anchors.left: parent.left;
        anchors.top: parent.top;
        anchors.leftMargin: 300;
        anchors.topMargin: 5;

        property color textColor: "transparent";
        property bool cheating: false;

        width: 50;
        height: 20;
        border.color: cheating ? "black" : "transparent";
        border.width: 2;
        radius: 5;

        Text {
            anchors.centerIn: parent;
            font.pointSize: 8;
            font.bold: false;
            text: "Cheater!";
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
                parent.textColor = parent.cheating ? "black" : "transparent";
            }

            onClicked: {
                parent.cheating = !parent.cheating;
                hexGrid.cheatMode = parent.cheating;
            }
        }
    }

    /// This controls the game speed. If too fast, it becomes clear that CPU
    /// struggles to keep up
    Slider {
        id: sldSpeed;

        anchors.right: parent.right;
        anchors.rightMargin: 10;
        anchors.topMargin: 8;

        width: 120;
        height: 20;

        minimumValue: -0.5;
        maximumValue: 2.5;

        value: 0;

        onValueChanged: hexGrid.gameSpeed = Math.pow(10, value);
    }

    /// Label showing the game speed next to the control above
    Text {
        anchors.right: sldSpeed.left;
        anchors.rightMargin: 10;
        anchors.verticalCenter: sldSpeed.verticalCenter;

        text: "Game speed:"
        font.pointSize: 10;
    }
}
