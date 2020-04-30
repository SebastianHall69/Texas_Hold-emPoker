#include "card.h"

Card::Card(Rank rank, Suit suit) : val(make_pair(rank, suit))
{

}

Card::Card(const Card &obj)
{
    val.first = obj.val.first;
    val.second = obj.val.second;
}

Card::~Card() {

}

void Card::setRank(Card::Rank rank)
{
    val.first = rank;
}

void Card::setSuit(Card::Suit suit)
{
    val.second = suit;
}

Card::Rank Card::getRank() const
{
    return val.first;
}

Card::Suit Card::getSuit() const
{
    return val.second;
}

string Card::toString() const
{
    return rankToString(val.first) + " of " + suitToString(val.second);
}

string Card::rankToString(Card::Rank rank)
{
    switch(rank) {
        case Ace: return "[ Ace";break;
        case Two: return "[ 2";break;
        case Three: return "[ 3";break;
        case Four: return "[ 4";break;
        case Five: return "[ 5";break;
        case Six: return "[ 6";break;
        case Seven: return "[ 7";break;
        case Eight: return "[ 8";break;
        case Nine: return "[ 9";break;
        case Ten: return "[ 10";break;
        case Jack: return "[ Jack";break;
        case Queen: return "[ Queen";break;
        case King: return "[ King";break;
        case BadRank:
        default: return "[ Bad Rank";
    }
}

string Card::suitToString(Card::Suit suit)
{
    switch(suit) {
        case Spade: return "Spades ]";break;
        case Heart: return "Hearts ]";break;
        case Diamond: return "Diamonds ]";break;
        case Club: return "Clubs ]";break;
        case BadSuit:
        default: return "Bad Suit ]";
    }
}

bool Card::operator<(const Card &obj) const
{
    if(this->val.first == obj.val.first) return this->val.second < obj.val.second;
    else return this->val.first < obj.val.first;
}

bool Card::operator==(const Card &obj) const
{
    return this->val.first == obj.val.first && this->val.second == obj.val.second;
}
