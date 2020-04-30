#include "hand.h"

Hand::Hand(vector<Card> hand)
{
    if(cards.size()) clearHand();
    setCards(hand);
}


string Hand::toString() const
{
    string output = "";
    for(set<Card>::const_iterator itr = cards.cbegin(); itr != cards.cend(); ++itr) {
        output += itr->toString() + " ";
    }
    return output;
}

void Hand::removeCard(int index)
{
    set<Card>::iterator itr = cards.begin();
    advance(itr, index);
    if(itr != cards.end()) {
        cards.erase(itr);
    }
}

void Hand::addCards(Card card)
{
    cards.insert(card);
}

void Hand::clearHand()
{
    cards.clear();
}

set<Card>* Hand::getCards()
{
    return &cards;
}

void Hand::setCards(vector<Card> hand)
{
    //Initialize hand with set of cards
    if(cards.size()) clearHand();
    for(vector<Card>::const_iterator itr = hand.cbegin(); itr != hand.cend(); ++itr) {
        cards.insert(*itr);
    }
}
