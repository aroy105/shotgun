#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "kuhn.hpp"

using shotgun::kuhn::Action;using shotgun::kuhn::Card;
using shotgun::kuhn::State;
using shotgun::kuhn::current_player;
using shotgun::kuhn::is_terminal;
using shotgun::kuhn::legal_actions;
using shotgun::kuhn::next_state;
using shotgun::kuhn::terminal_utility;

TEST_CASE("Root and intermediate histories are non-terminal") {
    REQUIRE_FALSE(is_terminal(State{Card::K, Card::Q, ""}));
    REQUIRE_FALSE(is_terminal(State{Card::K, Card::Q, "c"}));
    REQUIRE_FALSE(is_terminal(State{Card::K, Card::Q, "b"}));
    REQUIRE_FALSE(is_terminal(State{Card::K, Card::Q, "cb"}));
}