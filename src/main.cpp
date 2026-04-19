#include <iostream>
#include <vector>

#include "kuhn.hpp"

using shotgun::kuhn::Action;
using shotgun::kuhn::Card;
using shotgun::kuhn::State;
using shotgun::kuhn::current_player;
using shotgun::kuhn::is_terminal;
using shotgun::kuhn::legal_actions;
using shotgun::kuhn::next_state;
using shotgun::kuhn::terminal_utility;

std::string action_to_string(Action action) {
    switch (action) {
        case Action::Check:
            return "Check";
        case Action::Bet:
            return "Bet";
        case Action::Call:
            return "Call";
        case Action::Fold:
            return "Fold";
    }

    return "Unknown";
}

void print_actions(const std::vector<Action>& actions) {
    for (Action action : actions) {
        std::cout << action_to_string(action) << " ";
    }
    std::cout << "\n";
}

int main() {
    State root{Card::K, Card::Q, ""};

    std::cout << "Root history: \"" << root.history << "\"\n";
    std::cout << "Is terminal: " << std::boolalpha << is_terminal(root) << "\n";
    std::cout << "Current player: " << current_player(root) << "\n";
    std::cout << "Legal actions: ";
    print_actions(legal_actions(root));

    State after_check = next_state(root, Action::Check);
    std::cout << "\nAfter check history: \"" << after_check.history << "\"\n";
    std::cout << "Current player: " << current_player(after_check) << "\n";
    std::cout << "Legal actions: ";
    print_actions(legal_actions(after_check));

    State after_check_bet = next_state(after_check, Action::Bet);
    std::cout << "\nAfter check-bet history: \"" << after_check_bet.history << "\"\n";
    std::cout << "Current player: " << current_player(after_check_bet) << "\n";
    std::cout << "Legal actions: ";
    print_actions(legal_actions(after_check_bet));

    State terminal_state{Card::K, Card::Q, "cc"};
    std::cout << "\nTerminal history: \"" << terminal_state.history << "\"\n";
    std::cout << "Is terminal: " << is_terminal(terminal_state) << "\n";
    std::cout << "Utility for Player 1: " << terminal_utility(terminal_state) << "\n";

    return 0;
}