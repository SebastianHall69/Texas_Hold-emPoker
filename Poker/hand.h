#ifndef HAND_H
#define HAND_H

#include "card.h"
#include <set>
#include <vector>

using namespace std;

class Hand
{
private:
    set<Card> cards;
    float winProb;
    int handStr;

public:
    Hand(vector<Card> hand = vector<Card>());//Initailize a hand from a vector
    string toString() const;//Output all cards as a string
    void removeCard(int index);//Erase a card
    void addCards(Card card);//Add community cards into the hand
    void clearHand();
    set<Card>* getCards();
    void setCards(vector<Card> hand);
    void setWinProb(float prob);
    float getWinProb() const;
    void setHandStr(int str);
    int getHandStr() const;
};

#endif // HAND_H
