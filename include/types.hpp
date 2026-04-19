#pragma once

#include <string>

namespace shotgun::kuhn {

enum class Card {
    J,
    Q,
    K
};

enum class Action {
    Check,
    Bet,
    Call, 
    Fold
};

struct State {
    Card p1_card;
    Card p2_card;
    std::string history;
};

}