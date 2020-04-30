#include "deck.h"
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <ctime>

Deck::Deck()
{
    srand(static_cast<unsigned int>(time(0)));
    for(Card::Rank r = Card::Ace; r <= Card::King; r = static_cast<Card::Rank>(r + 1))
        for(Card::Suit s = Card::Spade; s <= Card::Club; s = static_cast<Card::Suit>(s + 1))
            deck.push_back(Card(r, s));

    shuffle();
}

void Deck::shuffle()
{
    random_shuffle(deck.begin(), deck.end());
}

Card Deck::drawCard()
{
    if(deck.empty()) return Card();
    Card card = *deck.begin();
    deck.erase(deck.begin());
    return card;
}

vector<Card> Deck::drawCards(int num)
{
    if(num < 1 || deck.begin() + num >= deck.end()) {
        return deck;
        deck.clear();
    } else {
        vector<Card> hand(deck.begin(), deck.begin() + num);
        deck.erase(deck.begin(), deck.begin() + num);
        return hand;
    }
}

int Deck::cardsRemaining() const
{
    return deck.size();
}
