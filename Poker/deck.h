#ifndef DECK_H
#define DECK_H

#include "card.h"
#include <vector>

class Deck
{
private:
    vector<Card> deck;

public:
    Deck();
    void shuffle();
    Card drawCard();
    vector<Card> drawCards(int num = 2);
    int cardsRemaining() const;
    Card at(int index) const;
};

#endif // DECK_H
