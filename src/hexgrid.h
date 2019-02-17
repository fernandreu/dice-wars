#ifndef HEXGRID_H
#define HEXGRID_H

#include "player.h"
#include "territory.h"
#include "hex.h"
#include "diceroll.h"

#include <QQuickItem>
#include <QtMath>
#include <QTimer>

class HexGrid : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(int gridWidth READ gridWidth WRITE setGridWidth)
    Q_PROPERTY(int gridHeight READ gridHeight WRITE setGridHeight)
    Q_PROPERTY(qreal radius READ radius WRITE setRadius)
    Q_PROPERTY(int numPlayers READ numPlayers WRITE setNumPlayers NOTIFY numPlayersChanged)
    Q_PROPERTY(int numTerritories READ numTerritories WRITE setNumTerritories)
    Q_PROPERTY(int territorySize READ territorySize WRITE setTerritorySize)
    Q_PROPERTY(int playerTurn READ playerTurn WRITE setPlayerTurn NOTIFY playerTurnChanged)
    Q_PROPERTY(bool cheatMode READ cheatMode WRITE setCheatMode)
    Q_PROPERTY(qreal gameSpeed READ gameSpeed WRITE setGameSpeed)

    Q_PROPERTY(QList<bool> humanList READ humanList WRITE setHumanList)

    int gridWidth_ = 60;
    int gridHeight_ = 40;
    qreal radius_;
    int numTerritories_ = 1;
    int territorySize_ = 20;

    int numPlayers_;

    /// Index indicating whose's turn is now
    int playerTurn_;

    int humansLeft_;

    int playersLeft_;

    Territory *selectedTerritory_, *otherTerritory_;
    QList<Player *> players_;
    QList<Territory *> territories_;
    QHash<QPair<int, int>, Hex *> map_;

    static const int DIRECTIONS[][2];

    void initMethodGrid();
    void initMethodGrowth();

    Hex *cubeRound(qreal x, qreal y, qreal z) const; //Returns the nearest hex cell from the given fractional x,y,z cube coordinates

    /// If this is true, the grid will stop responding to click events
    bool playingAnimation_ = false;

    /// Timer for general purpose in the game
    QTimer timer_;

    /// The interval between increasing the amount of dice by one, in milliseconds
    static const int GROWTH_INTERVAL = 30;

    /// The interval to start an AI turn
    static const int AI_STEP_INTERVAL = 500;

    /// The interval to select territories for the AI
    static const int AI_SELECT_INTERVAL = 300;

    /// The interval to process the attack
    static const int AI_ATTACK_INTERVAL = 300;

    /// Factor to multiply all the intervals to speed up / slow the processes
    qreal gameSpeed_ = 1;

    /// A little bit of cheating
    bool cheatMode_ = false;

    /// Play automatically for a human player as if it was an AI player. It is disconnected after the turn ends
    bool autoMode_ = false;

    QList<bool> humanList_;

    DiceRoll *diceRoll_ = nullptr;

public:
    explicit HexGrid(QQuickItem *parent = nullptr);
    ~HexGrid();

    int gridWidth() const;
    void setGridWidth(int gridWidth);

    int gridHeight() const;
    void setGridHeight(int gridHeight);

    qreal radius() const;
    void setRadius(qreal radius);

    int numPlayers() const;
    void setNumPlayers(int numPlayers);

    QHash<QPair<int, int>, Hex *> map() const;

    Hex *neighbour(Hex *hex, int direction) const;

    int numTerritories() const;
    void setNumTerritories(int numTerritories);

    int territorySize() const;
    void setTerritorySize(int territorySize);

    int playerTurn() const;
    void setPlayerTurn(int playerTurn);

    bool cheatMode() const;
    void setCheatMode(bool cheatMode);

    qreal gameSpeed() const;
    void setGameSpeed(const qreal &gameSpeed);

    QList<bool> humanList() const;
    void setHumanList(const QList<bool> &humanList);

signals:
    void showAttackResult(int attack, int defense);
    void connTerrChanged(int player, int connTerr);
    void numPlayersChanged();
    void playerTurnChanged();
    void victory(int player, bool human);

public slots:
    void initializeGrid();
    void processClick(qreal x, qreal y);
    void endTurn();

    bool isPlayerHuman(int index) const;

    //Mainly handles the timer when an AI turn starts. If it is called for a human player, autoMode_ will be set to true until the turn ends
    void startAITurn();

private slots:
    //Starts the next step for AI players
    void nextAIStep();

    //Handles the "animation" when dice are being added to a player at the end of a turn
    void growPlayer();

    //Selects the territories before starting an attack. This will be called twice, for the own and the neighbour territories. Then, it will start the timer for processAttack
    void selectTerritories();

    //Performs the attack, the aftermath, and starts again the timer for nextAIStep
    //TODO: Include an intermediate animation for this
    void processAttack() const;

    //Determines the conclusions of the attack once the dice have been rolled
    void attackFinished(int attack, int defense);
};

#endif // HEXGRID_H
