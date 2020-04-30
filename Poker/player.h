#ifndef PLAYER_H
#define PLAYER_H

#include "hand.h"
#include <map>
using namespace std;

class Player
{
public:
    enum Move {
        Call,
        Check,
        Raise,
        AllIn,
        Fold,
        Default
    };

    enum Status {
        Dealer,
        SmallBlind,
        BigBlind,
        Out,
        None
    };

private:
    Hand hand;//Players hand
    int chips;
    int betThisRound;
    map<string, string> playerInfo;
    Move move;
    Status status;

public:
    Player(string playerName = "Player", vector<Card> cards = vector<Card>(), int chips = 100, Move move = Default, Status status = None);
    ~Player();

    int getChips() const;
    void setChips(int value);
    void addChips(int value);
    int removeChips(int value);
    string getName() const;
    void setName(string value);
    set<Card>* getHand();
    void setHand(vector<Card> cards);
    Player::Move getMove() const;
    void setMove(const Player::Move &value);
    Player::Status getStatus() const;
    void setStatus(const Player::Status &value);
    string getPlayerInfo() const;
    string handToString() const;
    int getBetThisRound() const;
    void setBetThisRound(int value);
    bool operator<(const Player &rhs) const;//Sort by chip amount
    static string moveToString(const Player::Move &m);
};

#endif // PLAYER_H
