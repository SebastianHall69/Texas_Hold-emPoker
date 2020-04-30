#ifndef CARD_H
#define CARD_H

#include <utility>
#include <string>
using namespace std;

class Card
{
public:
    enum Rank {
        Ace = 1,
        Two = 2,
        Three = 3,
        Four = 4,
        Five = 5,
        Six = 6,
        Seven = 7,
        Eight = 8,
        Nine = 9,
        Ten = 10,
        Jack = 11,
        Queen = 12,
        King = 13,
        BadRank = -1
    };
    enum Suit {
        Spade = 1,
        Heart = 2,
        Diamond = 3,
        Club = 4,
        BadSuit = -1
    };

private:
    pair<Rank, Suit> val;

public:
    Card(Rank rank = BadRank, Suit suit = BadSuit);
    Card(const Card &obj);
    ~Card();
    void setRank(Rank rank);
    void setSuit(Suit suit);
    Rank getRank() const;
    Suit getSuit() const;
    string toString() const;
    static string rankToString(Rank rank);
    static string suitToString(Suit suit);
    bool operator<(const Card &obj) const;
    bool operator==(const Card &obj) const;
};

#endif // CARD_H
