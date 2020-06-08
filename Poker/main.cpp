#include <iostream>
#include <iomanip>
#include <string>
#include <cstring>
#include <limits>
#include <list>
#include <stack>
#include <queue>
#include <unordered_map>
#include <algorithm>

#include "player.h"
#include "deck.h"
using namespace std;


typedef list<Player> lp;
typedef list<Player>::iterator lpi;


int getNumPlayers();
void clearInputBuffer();
lp createPlayers(int numPlayers);
lpi nextPlayer(lp *players, lpi from);
lpi previousPlayer(lp *players, lpi from);
stack<string> getTitles();
void showShared(queue<Card> sharedCards);
lpi playPhase(lp *players, lpi player, lpi dealer, int &minBet, int &pot);
bool allMinBet(lp *players, int minBet);
int handval(vector<Card>);
void simWinProb(lp *players, queue<Card> shared, Deck *deck);
lpi getWinner(lp *players, queue<Card> shared);
string randName();
void recSort(string arr[], int n);


int main() {
    srand(static_cast<unsigned int>(time(0)));
    //Declare variables
    int numPlayers;
    lp players;
    lpi player, dealer, smallBlind, bigBlind;
    bool contPlay = true;
    priority_queue<Player> results;

    //Get number of players
    numPlayers = getNumPlayers();

    //Get name from user and create players
    players = createPlayers(numPlayers);

    //Keep iterator to human player, blinds, and dealer
    player = players.begin();
    dealer = players.begin();
    smallBlind = previousPlayer(&players, dealer);
    bigBlind = previousPlayer(&players, smallBlind);


    //While number of players > 0 and player wants to continue playing, this is the start of each round
    while(players.size() > 1 && contPlay) {
        //Declare and initialize variable
        int pot = 0, minBet = 0;
        stack<string> titles = getTitles();//Create titles
        Deck deck;//Creates deck
        queue<Card> sharedCards;
        lpi roundWinner = players.end();

        //Deal cards to each players, reset statuses
        for(lpi itr = players.begin(); itr != players.end(); ++itr) {
            itr->setHand(deck.drawCards(2));
            itr->setMove(Player::Default);
            itr->setStatus(Player::None);
            itr->setBetThisRound(0);
        }

        //Set statuses for dealers and blinds
        dealer->setStatus(Player::Dealer);
        smallBlind->setStatus(Player::SmallBlind);
        bigBlind->setStatus(Player::BigBlind);

        //Take blinds from players
        int bb = bigBlind->removeChips(10), sb = smallBlind->removeChips(5);
        minBet = max(bb, sb);
        pot = bb + sb;

        //Output blinds
        cout<<"\n\n"<<titles.top()<<endl;
        titles.pop();
        cout<<"Big Blind: "<<bigBlind->getName()<<" "<<bb<<" chips\n";
        cout<<"Small Blind: "<<smallBlind->getName()<<" "<<sb<<" chips\n";


        //Play pre flop through river
        for(int i = 0; i < 4; ++i) {
            //Output title
            cout<<"\n\n"<<titles.top()<<endl;
            titles.pop();

            //Output community cards
            cout<<"Community Cards: ";
            showShared(sharedCards);

            //Calculate win probability for each player
            simWinProb(&players, sharedCards, &deck);

            //Play round function, get iterator to round winner returned
            roundWinner = playPhase(&players, player, dealer, minBet, pot);
            if(roundWinner != players.end()) break;//Break out of loop, default winner found            

            //Draw community cards. 3 on first loop, 0 on 3rd loop
            if(i == 0) for(int j = 0; j < 3; ++j) sharedCards.push(deck.drawCard());
            else if(i < 3) sharedCards.push(deck.drawCard());
        }

        //Showdown, clear all titles except last one
        while(titles.size() > 1) {
            titles.pop();
        }
        cout<<"\n\n"<<titles.top()<<endl;
        titles.pop();

        //Output winner
        if(roundWinner == players.end()) {//Determine who won by hands
            roundWinner = getWinner(&players, sharedCards);
        }
        cout<<"Winner: "<<roundWinner->getName()<<endl;
        cout<<"Winning Hand: "<<roundWinner->handToString()<<" +\nCommunity: ";
        showShared(sharedCards);
        roundWinner->addChips(pot);
        cout<<endl;
        
        //Show player their chips
        cout<<"Players chips: "<<player->getChips()<<endl<<endl;

        //Mark all those with 0 chips as out
        for(lpi itr = players.begin(); itr != players.end(); ++itr) {
            if(itr->getChips() <= 0) {
                itr->setStatus(Player::Out);
                results.push(*itr);
            }
        }

        //If player is out then set condition flag to break loop
        if(player->getStatus() == Player::Out)
            contPlay = false;

        //Rotate dealers and blinds
        dealer = previousPlayer(&players, dealer);//Left of current dealer
        smallBlind = previousPlayer(&players, dealer);//Left of new dealer
        bigBlind = previousPlayer(&players, smallBlind);//Left of small blind


        //Delete out players from list
        for(lpi itr = players.begin(); itr  != players.end();) {
            itr = (itr->getStatus() == Player::Out) ? players.erase(itr) : ++itr;
        }


        //If flag not set to break game loop ask player if they want to leave or continue
        if(contPlay) {
            char c;
            cout<<"Enter anything to continue playing or q to quit: ";
            cin>>c;
            if(tolower(c) == 'q') contPlay = false;
        }
    }

    //Empty remaining players from players list into results priority queue
    for(lpi itr = players.begin(); itr != players.end(); ++itr) {
        results.push(*itr);
    }

    //Output results
    while(results.size() > 0) {
        cout<<setw(20)<<setfill('.')<<left<<results.top().getName();
        cout<<results.top().getChips()<<" Chips\n";
        results.pop();
    }
    return 0;
}

//Plays one betting phase of the game for all non folded or out players
lpi playPhase(lp *players, lpi player, lpi dealer, int &minBet, int &pot) {
    lpi current = dealer;
    lpi roundWinner = players->end();
    int remaining = players->size(), amntBet = 0;
    bool flag = true;

    while(flag) {
        //Change current player
        current = previousPlayer(players, current);

        //Play round if not folded or all in
        if(!(current->getMove() == Player::Fold || current->getMove() == Player::AllIn)) {
            amntBet = minBet - current->getBetThisRound();

            //Human Players Turn
            if(current == player) {
                bool call = (amntBet > 0);
                cout<<"\n\nYour Turn, Your Chips: "<<current->getChips()<<", Pot: "<<pot<<endl;
                cout<<"Your Cards: "<<current->handToString()<<endl;

                //Output choices
                cout<<((call) ? "1.) Call " + to_string(amntBet) : "1.) Check")<<endl;
                cout<<"2.) Raise 10\n";
                cout<<"3.) Raise 20\n";
                cout<<"4.) All In\n";
                cout<<"5.) Fold\n";

                //Get choices
                int choice = -1;
                do {
                    cout<<"Your Choice: ";
                    cin>>choice;
                    if(cin.fail()) clearInputBuffer();
                } while(choice < 1 || choice > 5);
                cout<<"\n\n";


                //Handle choice
                switch(choice) {
                case 1: {
                    current->setMove( (call) ? Player::Call : Player::Check);
                    amntBet = current->removeChips(amntBet);
                    pot += amntBet;
                    break;
                }
                case 2: {
                    current->setMove(Player::Raise);
                    int val = current->removeChips(amntBet + 10);
                    pot += val;
                    amntBet = max(current->getBetThisRound() - minBet, 0);
                    minBet += amntBet;
                    break;
                }
                case 3: {
                    current->setMove(Player::Raise);
                    int val = current->removeChips(amntBet + 20);
                    pot += val;
                    amntBet = max(current->getBetThisRound() - minBet, 0);
                    minBet += amntBet;
                    break;
                }
                case 4: {
                    current->setMove(Player::AllIn);
                    amntBet = current->removeChips(current->getChips());
                    pot += amntBet;
                    amntBet = 0;
                    break;
                }
                case 5: {
                    current->setMove(Player::Fold);
                    amntBet = 0;
                    break;
                }
                default: {
                    current->setMove(Player::Default);
                    break;
                }
                }

            } else {//AI Players Turn
                float potOdds = static_cast<float>(minBet) / (minBet + pot);
                float ror = current->handRef()->getWinProb() / potOdds;
                bool fold = false, raise = false, call = false;
                int randChoice = rand() % 100;

                //AI choice based on rate of return from pot odds and hand strength
                if(ror < 0.4) {
                    if(randChoice < 95) {
                        fold = true;
                    } else {
                        raise = false;
                    }
                } else if(ror < 0.8) {
                    if(randChoice < 80) {
                        fold = true;
                    } else if(randChoice < 85) {
                        call = true;
                    } else {
                        raise = true;
                    }
                } else if(ror < 1.2) {
                    if(randChoice < 60) {
                        call = true;
                    } else {
                        raise = true;
                    }
                } else {
                    if(randChoice < 30) {
                        call = true;
                    } else {
                        raise = true;
                    }
                }

                //Don't fold unless necessary
                if(fold && amntBet == 0) {
                    fold = false;
                    call = true;
                }


                //Handle move
                if(call) {
                    current->setMove((amntBet) ? Player::Call : Player::Check);
                    amntBet = current->removeChips(amntBet);
                    pot += amntBet;
                    if(current->getChips() == 0) current->setMove(Player::AllIn);
                } else if(raise) {
                    current->setMove(Player::Raise);
                    int val = current->removeChips(amntBet + 20);
                    pot += val;
                    amntBet = max(current->getBetThisRound() - minBet, 0);
                    minBet += amntBet;
                    if(current->getChips() == 0) current->setMove(Player::AllIn);
                } else if(fold) {
                    current->setMove(Player::Fold);
                    amntBet = 0;
                }
            }
        }

        //Output player status
        cout<<current->getPlayerInfo()<<((amntBet > 0) ? to_string(amntBet) : "")<<endl<<endl;

        //If only one player remaining find winner by default
        if(remaining == 1) {
            flag = false;
            for(lpi itr = players->begin(); itr != players->end(); ++itr) {
                if(itr->getMove() != Player::Fold) {
                    roundWinner = itr;
                }
            }
        } else if(current == dealer && allMinBet(players, minBet)) {
            flag = false;
        }
    }

    return roundWinner;
}

//Checks if all players have reached the minimum bet or are out for the round
bool allMinBet(lp *players, int minBet) {
    for(lp::const_iterator itr = players->cbegin(); itr != players->cend(); ++itr) {
        if(!(itr->getMove() == Player::Fold || itr->getMove() == Player::AllIn))
            if(itr->getBetThisRound() < minBet)
                return false;
    }
    return true;
}

//Gets number of AI players to play with the player
int getNumPlayers() {
    int numPlayers = -1;
    cout<<"Welcome To Poker\n";
    do {
        cout<<"\nEnter The Number Of AI Opponents [1-5]: ";
        cin>>numPlayers;
        if(cin.fail()) clearInputBuffer();
        else cin.ignore();
    } while(numPlayers < 1 || numPlayers > 5);
    cout<<"\n";

    return numPlayers;
}

//Clears input buffer after bad input attempt
void clearInputBuffer(){
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

//Creates the list of players with human player as first player
lp createPlayers(int numPlayers) {
    lp players;
    string name;

    do {
        cout<<"\nEnter Your Name: ";
        getline(cin, name);

    } while(name.empty());

    players.push_back(Player(name));

    for(int i = 0; i < numPlayers; ++i) {
        players.push_back(Player(randName()));
    }

    return players;
}

//Cycles to next player in round
lpi nextPlayer(lp *players, lpi from) {
    lpi current = from;
    do {
        ++current;
        if(current == players->end()) current = players->begin();
    } while(current->getStatus() == Player::Out);

    return current;
}

//Cycles to previous player in round
lpi previousPlayer(lp *players, lpi from) {
    lpi current = from;
    do {
        if(current == players->begin()) current = players->end();
        --current;
    } while(current->getStatus() == Player::Out);

    return current;
}

//Returns a string of titles in a stack. I should have used a file to read in but I didn't because I'm not lame
stack<string> getTitles() {
    stack<string> s;
    s.push("------------------------------------Showdown------------------------------------");
    s.push("--------------------------------------River-------------------------------------");
    s.push("--------------------------------------Turn--------------------------------------");
    s.push("--------------------------------------Flop--------------------------------------");
    s.push("------------------------------------Pre Flop------------------------------------");
    s.push("-------------------------------------Blinds-------------------------------------");

    return s;
}

//Displays the shared community cards that are in the queue
void showShared(queue<Card> sharedCards) {
    while(!sharedCards.empty()) {
        cout<<sharedCards.front().toString()<<" ";
        sharedCards.pop();
    }
    cout<<endl<<endl;
}

//Evaluates a hand and returns its value as an integer
int handval(vector<Card> v) {
    //Declare variables
    int suit[4], rank[13], pair = 0, cont = 0, highCard = 0;
    int pairCard1 = 0, pairCard2 = 0, straightCard = 0, threeCard = 0, fourCard = 0, flushSuit = 0;
    bool flush = false, straight = false, fourKind = false, threeKind = false;

    //Initialize arrays
    memset(suit, 0, 4 * sizeof(int));
    memset(rank, 0, 13 * sizeof(int));

    //Count num of each rank and suit
    for(int i = 0; i < v.size(); ++i) {
        ++suit[v[i].getSuit()];
        ++rank[v[i].getRank()];
    }

    //Check suits for flush
    for(int i = 0; i < 4; ++i) {
        if(suit[i] > 4) {
            flush = true;
            flushSuit = i;
            break;
        }
    }

    //Check ranks
    for(int i = 0; i < 13; ++i) {

        //Check for straight, also high card
        if(rank[i]) {
            ++cont;
            highCard = i;
            if(cont > 4) {
                straightCard = i;
                straight = true;
            }
        } else {
            cont = 0;
        }

        //Check for counts of cards
        if(rank[i] == 4) {//Check for four of a kind
            fourKind = true;
            fourCard = i;
        } else if(rank[i] == 3) {//Check for three of a kind
            threeKind = true;
            threeCard = i;
        } else if(rank[i] == 2) {//Count pairs
            ++pair;
            pairCard2 = pairCard1;
            pairCard1 = i;
        }
    }

    //Evaluate best hand, giving a numeric value for better hands
    if(straight && flush) {//Straight flush possible, not guaranteed. I'm saying it is anyway
        return 8000000 + straightCard * 10000;
    } else if(fourKind) {//Four of a kind
        return 7000000 + fourCard * 10000 + highCard;
    } else if(threeKind && pair) {//Full house
        return 6000000 + threeKind * 10000 + pairCard1 * 100;
    } else if(flush) {//Flush
        int val = 5000000;
        int cards[5];
        for(int i = 0, cnt = 0; i < v.size() && cnt < 5; ++i) {
            if(v[i].getSuit() == flushSuit) cards[cnt++] = v[i].getRank();
        }//There is barely enough to divide each time by 13 to get right score
        sort(cards, cards + 5, greater<int>());
        for(int i = 0, mult = 1000000 / 13; i < 5; ++i) {
            val += mult * cards[i];
            mult /= 13;
        }
        return val;

    } else if(straight) {//Straight
        return 4000000 + straightCard * 10000;
    } else if(threeKind) {//Three of a kind
        return 3000000 + threeCard * 10000 + highCard;
    } else if(pair > 1) {//Two pair
        return 2000000 + pairCard1 * 10000 + pairCard2 * 100 + highCard;
    } else if(pair) {//One pair
        return 1000000 + pairCard1 * 10000 + highCard;
    } else {//High card
        return highCard;
    }
}

//Get hand strength of each player
void simWinProb(lp *players, queue<Card> shared, Deck *deck) {
    int ind = 0, loops = 1000, extraCards = 5 - shared.size();
    vector<Card> v;

    //Push community cards into vector
    while(!shared.empty()) {
        v.push_back(shared.front());
        shared.pop();
    }

    //Loop 1000 times simulating 1000 games
    for(int i = 0; i < loops; ++i) {

        //Fill remaining cards from deck
        ind = rand() % deck->cardsRemaining();
        for(int j = 0; j < extraCards; ++j) {
            v.push_back(deck->at(ind % deck->cardsRemaining()));
            ++ind;
        }

        //Get hand strength for each player
        for(lpi itr = players->begin(); itr != players->end(); ++itr) {
            if(itr->getMove() != Player::Fold) {//Get players hand strength
                //Push players hand into vector to test strength for total of 7 cards
                v.push_back(*(itr->getHand()->begin()));
                v.push_back(*(++(itr->getHand()->begin())));

                //Set hand value for each player
                itr->handRef()->setHandStr(handval(v));

                //Remove two added cards
                v.pop_back();
                v.pop_back();

            } else {//Else set the users hand to low
                itr->handRef()->setHandStr(-1);
            }
        }

        //Remove random cards from deck
        for(int j = 0; j < extraCards; ++j) {
            v.pop_back();
        }

        //See who won the hand, increment their wins
        lpi winner = players->begin();
        for(lpi itr = players->begin(); itr != players->end(); ++itr) {
            if(itr->handRef()->getHandStr() > winner->handRef()->getHandStr()) {
                winner = itr;
            }
        }
        ++winner->wins;
    }

    //Set the win probabilities for each player, reset wins
    for(lpi itr = players->begin(); itr != players->end(); ++itr) {
        itr->handRef()->setWinProb(static_cast<float>(itr->wins) / loops);
        itr->wins = 0;
    }
}

lpi getWinner(lp *players, queue<Card> shared) {
    vector<Card> v;

    //Add shared cards to deck
    while(!shared.empty()) {
        v.push_back(shared.front());
        shared.pop();
    }

    //Get hand strength for each player
    for(lpi itr = players->begin(); itr != players->end(); ++itr) {
        if(itr->getMove() != Player::Fold) {//Get players hand strength
            //Push players hand into vector to test strength for total of 7 cards
            v.push_back(*(itr->getHand()->begin()));
            v.push_back(*(++(itr->getHand()->begin())));

            //Set hand value for each player
            itr->handRef()->setHandStr(handval(v));

            //Remove two added cards
            v.pop_back();
            v.pop_back();

        } else {//Else set the users hand to low
            itr->handRef()->setHandStr(-1);
        }
    }

    lpi winner = players->begin();
    for(lpi itr = players->begin(); itr != players->end(); ++itr) {
        if(itr->handRef()->getHandStr() > winner->handRef()->getHandStr()) {
            winner = itr;
        }
    }

    return winner;
}

//Recursive bubble sort
void recSort(string arr[], int n) {
    if (n == 1) return;

    for (int i=0; i<n-1; i++)
        if (arr[i] > arr[i+1])
            swap(arr[i], arr[i+1]);

    recSort(arr, n-1);
}

string randName() {
    int size = 23;
    string first[] = {"Steve", "Onson", "Darryl", "Anatoli", "Rey", "GlenAllen", "Mario", "Raul", "Kevin", "Tony", "Bobson", "Willie", "Jeromy", "Scott", "Shown", "Dean", "Mike", "Dwigt", "Tim", "Karl", "Mike", "Todd", "Jeromaldo"};
    string last[] = {"McDicheal", "Sweemey", "Archideld", "Smorin", "McScriff", "Mixon", "McRlwain", "Chamgerlain", "Nogilny", "Smehrik", "Dugnutt", "Dustice", "Gride", "Dourque", "Furcotte", "Wesrey", "Truk", "Rortugal", "Sandaele", "Dandleton", "Serandez", "Bonzalez", "Flacikkk"};

    //Recursively sort arrays
    recSort(first, size);
    recSort(last, size);

    //100% Hashing expert
    unordered_map<string, string> m;
    m["first"] = first[rand() % size];
    m["second"] = last[rand() % size];

    return m["first"] + " " + m["second"];
}
