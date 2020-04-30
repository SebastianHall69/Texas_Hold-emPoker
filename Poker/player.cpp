#include "player.h"

Player::Player(string playerName, vector<Card> cards, int chips, Move move, Status status) : hand(cards), chips(chips), betThisRound(0), move(move)
{
    setName(playerName);
    setStatus(status);
}

Player::~Player()
{

}

int Player::getChips() const
{
    return chips;
}

void Player::setChips(int value)
{
    chips = value;
}

void Player::addChips(int value)
{
    chips += value;
}

int Player::removeChips(int value)
{
    int paid = min(value, chips);
    chips -= paid;
    if(chips == 0) {
        move = Player::AllIn;
    }

    betThisRound += paid;
    return paid;
}

string Player::getName() const
{
    return (playerInfo.count("name")) ? playerInfo.at("name") : "Default Name";
}

void Player::setName(string value)
{
    playerInfo["name"] = value;
}

set<Card>* Player::getHand()
{
    return hand.getCards();
}

void Player::setHand(vector<Card> cards)
{
    hand.setCards(cards);
}

Player::Move Player::getMove() const
{
    return move;
}

void Player::setMove(const Player::Move &value)
{
    move = value;
}

Player::Status Player::getStatus() const
{
    return status;
}

void Player::setStatus(const Player::Status &value)
{
    //Set status
    status = value;

    //Update player info
    switch(value) {
    case Dealer: playerInfo["dealer"] = "Dealer"; break;
    case SmallBlind: playerInfo["smallBlind"] = "Small Blind"; break;
    case BigBlind: playerInfo["bigBlind"] = "Big Blind"; break;
    default: {
        if(playerInfo.count("dealer")) playerInfo.erase(playerInfo.find("dealer"));
        if(playerInfo.count("smallBlind")) playerInfo.erase(playerInfo.find("smallBlind"));
        if(playerInfo.count("bigBlind")) playerInfo.erase(playerInfo.find("bigBlind"));
        break;
    }
    }
}

string Player::getPlayerInfo() const
{
    //Add name to string
    string output = getName() + ", ";

    //Add statuses to string
    if(playerInfo.size() > 1) {
        for(map<string, string>::const_iterator itr = playerInfo.cbegin(); itr != playerInfo.cend(); ++itr) {
            if(itr->first != "name")
                output += itr->second + ", ";
        }
    }

    //Add chips remaining to string
    output += "Chips: " + to_string(chips);

    if(this->move != Player::Default) {
        output += "\n\tMove: " + moveToString(move) + " ";
    }

    return output;
}

string Player::handToString() const
{
    return hand.toString();
}

int Player::getBetThisRound() const
{
    return betThisRound;
}

void Player::setBetThisRound(int value)
{
    betThisRound = value;
}

bool Player::operator<(const Player &rhs) const
{
    return this->chips < rhs.chips;
}

string Player::moveToString(const Player::Move &m)
{
    switch(m) {
    case Call: return "Call"; break;
    case Check: return "Check"; break;
    case Raise: return "Raise"; break;
    case AllIn: return "All In"; break;
    case Fold: return "Fold"; break;
    case Default:
    default: return "Default";
    }
}

