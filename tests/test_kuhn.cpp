#include <array>
#include <string_view>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "kuhn.hpp"

using shotgun::kuhn::Action;
using shotgun::kuhn::Card;
using shotgun::kuhn::State;
using shotgun::kuhn::current_player;
using shotgun::kuhn::is_terminal;
using shotgun::kuhn::legal_actions;
using shotgun::kuhn::next_state;
using shotgun::kuhn::terminal_utility;
using shotgun::kuhn::infoset_key;

static const char* to_string(Card c) {
    switch (c) {
        case Card::K: return "K";
        case Card::Q: return "Q";
        case Card::J: return "J";
    }
    return "?";
}

TEST_CASE("Root + intermediate histories are notated as non-terminal") {
    constexpr std::array<Card, 3> cards{{Card::K, Card::Q, Card::J}};
    constexpr std::array<std::string_view, 4> histories{{"", "c", "b", "cb"}};

    for (Card c1 : cards) {
        for (Card c2: cards) {
            if (c1 == c2) continue;

            for (auto h : histories) {
                DYNAMIC_SECTION("cards=(" << to_string(c1) << "," << to_string(c2)
                                          << "), history=\"" << h << "\"") {
                    REQUIRE_FALSE(is_terminal(State{c1, c2, std::string(h)}));
                }
            }
        }
    }
}

TEST_CASE("Terminal histories are notated correctly") {
    constexpr std::array<Card, 3> cards{{Card::K, Card::Q, Card::J}};
    constexpr std::array<std::string_view, 5> histories{{"cc", "bf", "bc", "cbf", "cbc"}};
    for (Card c1 : cards) {
        for (Card c2: cards) {
            if (c1 == c2) continue;

            for (auto h : histories) {
                DYNAMIC_SECTION("cards=(" << to_string(c1) << "," << to_string(c2)
                                          << "), history=\"" << h << "\"") {
                    REQUIRE(is_terminal(State{c1, c2, std::string(h)}));
                }
            }
        }
    }
}

TEST_CASE("Legal actions from root are only check/bet") {
    State state{Card::K, Card::Q, ""};

    REQUIRE(legal_actions(state) == std::vector<Action>{
        Action::Check,
        Action::Bet
    });
}

TEST_CASE("Legal actions after initial check are check/bet") {
    State state{Card::K, Card::Q, "c"};

    REQUIRE(legal_actions(state) == std::vector<Action>{
        Action::Check,
        Action::Bet
    });
}

TEST_CASE("Legal actions after open bet are call/fold") {
    State state{Card::K, Card::Q, "b"};

    REQUIRE(legal_actions(state) == std::vector<Action>{
        Action::Call,
        Action::Fold
    });
}

TEST_CASE("Legal actions after check-bet are call/fold") {
    State state{Card::K, Card::Q, "cb"};

    REQUIRE(legal_actions(state) == std::vector<Action>{
        Action::Call,
        Action::Fold
    });
}

TEST_CASE("No legal actions at terminal state") {
    constexpr std::array<Card, 3> cards{{Card::K, Card::Q, Card::J}};
    constexpr std::array<std::string_view, 5> histories{{"cc", "bf", "bc", "cbf", "cbc"}};
    for (Card c1 : cards) {
        for (Card c2: cards) {
            if (c1 == c2) continue;

            for (auto h : histories) {
                DYNAMIC_SECTION("cards=(" << to_string(c1) << "," << to_string(c2)
                                          << "), history=\"" << h << "\"") {
                    REQUIRE(legal_actions(State{c1, c2, std::string(h)}).empty());
                }
            }
        }
    }
}

TEST_CASE("next_state appends check correctly") {
    State root{Card::K, Card::Q, ""};

    State next = next_state(root, Action::Check);

    REQUIRE(next.p1_card == Card::K);
    REQUIRE(next.p2_card == Card::Q);
    REQUIRE(next.history == "c");
}

TEST_CASE("next_state appends bet correctly") {
    State root{Card::K, Card::Q, ""};

    State next = next_state(root, Action::Bet);

    REQUIRE(next.history == "b");
}

TEST_CASE("next_state appends call correctly") {
    State state{Card::K, Card::Q, "b"};

    State next = next_state(state, Action::Call);

    REQUIRE(next.history == "bc");
}

TEST_CASE("next_state appends fold correctly") {
    State state{Card::K, Card::Q, "cb"};

    State next = next_state(state, Action::Fold);

    REQUIRE(next.history == "cbf");
}

TEST_CASE("Current player is player 1 at root") {
    REQUIRE(current_player(State{Card::K, Card::Q, ""}) == 0);
}

TEST_CASE("Current player is player 2 after player 1 checks") {
    REQUIRE(current_player(State{Card::K, Card::Q, "c"}) == 1);
}

TEST_CASE("Current player is player 2 after player 1 bets") {
    REQUIRE(current_player(State{Card::K, Card::Q, "b"}) == 1);
}

TEST_CASE("Current player is player 1 after check-bet") {
    REQUIRE(current_player(State{Card::K, Card::Q, "cb"}) == 0);
}

TEST_CASE("Check-check showdown utility is plus or minus 1") {
    REQUIRE(terminal_utility(State{Card::K, Card::Q, "cc"}) == 1);
    REQUIRE(terminal_utility(State{Card::Q, Card::K, "cc"}) == -1);
    REQUIRE(terminal_utility(State{Card::Q, Card::J, "cc"}) == 1);
    REQUIRE(terminal_utility(State{Card::J, Card::Q, "cc"}) == -1);
}

TEST_CASE("Bet-fold utility is always plus 1 for player 1") {
    REQUIRE(terminal_utility(State{Card::J, Card::K, "bf"}) == 1);
    REQUIRE(terminal_utility(State{Card::Q, Card::J, "bf"}) == 1);
    REQUIRE(terminal_utility(State{Card::K, Card::Q, "bf"}) == 1);
}

TEST_CASE("Check-bet-fold utility is always minus 1 for player 1") {
    REQUIRE(terminal_utility(State{Card::J, Card::K, "cbf"}) == -1);
    REQUIRE(terminal_utility(State{Card::Q, Card::J, "cbf"}) == -1);
    REQUIRE(terminal_utility(State{Card::K, Card::Q, "cbf"}) == -1);
}

TEST_CASE("Bet-call showdown utility is plus or minus 2") {
    REQUIRE(terminal_utility(State{Card::K, Card::Q, "bc"}) == 2);
    REQUIRE(terminal_utility(State{Card::Q, Card::K, "bc"}) == -2);
    REQUIRE(terminal_utility(State{Card::Q, Card::J, "bc"}) == 2);
    REQUIRE(terminal_utility(State{Card::J, Card::Q, "bc"}) == -2);
}

TEST_CASE("Check-bet-call showdown utility is plus or minus 2") {
    REQUIRE(terminal_utility(State{Card::K, Card::Q, "cbc"}) == 2);
    REQUIRE(terminal_utility(State{Card::Q, Card::K, "cbc"}) == -2);
    REQUIRE(terminal_utility(State{Card::Q, Card::J, "cbc"}) == 2);
    REQUIRE(terminal_utility(State{Card::J, Card::Q, "cbc"}) == -2);
}

TEST_CASE("infoset_key function returns correct value") {
    REQUIRE(infoset_key(State{Card::K, Card::Q, ""}) == "K|");
    REQUIRE(infoset_key(State{Card::J, Card::Q, ""}) == "J|");
    REQUIRE(infoset_key(State{Card::Q, Card::J, "c"}) == "J|c");
    REQUIRE(infoset_key(State{Card::K, Card::Q, "cb"}) == "K|cb");
}