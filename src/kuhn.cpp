#include "kuhn.hpp"

namespace shotgun::kuhn {

bool is_terminal(const State& state) {
    return state.history == "cc" ||
           state.history  == "bf" ||
           state.history  == "bc" ||
           state.history  == "cbf" ||
           state.history  == "cbc";
}

std::vector<Action> legal_actions(const State& state) {
    if (state.history.empty()) {
        return {Action::Check, Action::Bet};
    }

    if (state.history == "c") {
        return {Action::Check, Action::Bet};
    }

    if (state.history == "b") {
        return {Action::Call, Action::Fold};
    }

    if (state.history == "cb") {
        return {Action::Call, Action::Fold};
    }

    return {};
}

State next_state(const State& state, Action action) {
    std::string next_history = state.history;

    switch(action) {
        case Action::Check:
            next_history += 'c';
            break;
        case Action::Bet:
            next_history += 'b';
            break;
        case Action::Call:
            next_history += 'c';
            break;
        case Action::Fold:
            next_history += 'f';
            break;  
    }

    return State{state.p1_card, state.p2_card, next_history};
}

int current_player(const State& state) {
    if (state.history.empty() || state.history == "cb") {
        return 0;
    }

    if (state.history == "c" || state.history == "b") {
        return 1;
    }

    return -1;
}

namespace {
    int card_rank(Card card) {
        switch (card) {
            case Card::J:
                return 0;
            case Card::Q:
                return 1;
            case Card::K:
                return 2;
        }

        return -1;
    }
}

int terminal_utility(const State& state) {
    if (state.history == "bf") return 1;

    if (state.history == "cbf") return -1;

    if (state.history == "cc") {
        return card_rank(state.p1_card) > card_rank(state.p2_card) ? 1: -1;
    }

    if (state.history == "bc" || state.history == "cbc") {
        return card_rank(state.p1_card) > card_rank(state.p2_card) ? 2: -2;
    }

    return 0;
}

}