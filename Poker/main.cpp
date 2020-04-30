#include <iostream>
#include <iomanip>
#include <string>
#include <limits>
#include <list>
#include <stack>
#include <queue>

#include "player.h"
#include "deck.h"


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

using namespace std;
int main() {
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
        cout<<"Winner is default player for now\n";
        roundWinner = player;
        if(roundWinner == players.end()) {//Determine who won by hands

        } else {//Default winner from rest folding
            cout<<"Winner by default: "<<roundWinner->getName()<<endl;
            roundWinner->addChips(pot);
        }
        cout<<"Winning hand: (all cards for now) "<<roundWinner->handToString()<<" ";
        showShared(sharedCards);
        
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



        //If flag not set to break game loop
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

            } else {
                if(rand() % 20 > 1) {
                    current->setMove((amntBet) ? Player::Call : Player::Check);
                    amntBet = current->removeChips(amntBet);
                    pot += amntBet;
                } else {
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


bool allMinBet(lp *players, int minBet) {
    for(lp::const_iterator itr = players->cbegin(); itr != players->cend(); ++itr) {
        if(!(itr->getMove() == Player::Fold || itr->getMove() == Player::AllIn))
            if(itr->getBetThisRound() < minBet)
                return false;
    }
    return true;
}


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

void clearInputBuffer(){
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

lp createPlayers(int numPlayers) {
    lp players;
    string name;

    do {
        cout<<"\nEnter Your Name: ";
        getline(cin, name);

    } while(name.empty());

    players.push_back(Player(name));

    for(int i = 0; i < numPlayers; ++i) {
        players.push_back(Player("AI " + to_string(i + 1)));
    }

    return players;
}

lpi nextPlayer(lp *players, lpi from) {
    lpi current = from;
    do {
        ++current;
        if(current == players->end()) current = players->begin();
    } while(current->getStatus() == Player::Out);

    return current;
}

lpi previousPlayer(lp *players, lpi from) {
    lpi current = from;
    do {
        if(current == players->begin()) current = players->end();
        --current;
    } while(current->getStatus() == Player::Out);

    return current;
}

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

void showShared(queue<Card> sharedCards) {
    while(!sharedCards.empty()) {
        cout<<sharedCards.front().toString()<<" ";
        sharedCards.pop();
    }
    cout<<endl<<endl;
}
