#include "cfr.hpp"

#include <algorithm>
#include <array>
#include <iostream>
#include <vector>

namespace shotgun::kuhn {

double CFRTrainer::cfr(const State& state, double p1_reach, double p2_reach) {
    // once we recursively reached a terminal node (showdown or fold) return utility
    // remember, reach probabilities are our odds of getting to some state
    // If player 1 bets at root w p=0.4, and we call with p=0.3 facing a bet
    // the reach probability of history "bc" due to strategic choices is p1_reach = 0.4, p2_reach = 0.3
    // and strategic reach is 0.12
    if (is_terminal(state)) {
        return terminal_utility(state);
    }
    // from our state we will derive our infoset
    std::string key = infoset_key(state);
    // node represents the infoset, like K|cb or J|, where we only know our card and actions taken
    // if no node exists, C++ creates a default one
    // default means regret_sum = strategy_sum = 0, 0
    Node& node = node_map_[key]; 
    // get the current strategy from the regrets, our actions, and init empty action utilities array
    // initially prior to any traversal of the tree, regret_sum and strategy_sum are initialized to 0, 0
    // and in our get_strategy code, if regret = 0, 0 and thus strategy = 0, 0
    // we return 0.5, 0.5 as our strategy
    std::array<double, 2> strategy = node.get_strategy();
    std::vector<Action> actions = legal_actions(state);
    std::array<double, 2> action_utilities{};

    int player = current_player(state);
    // for each of the two actions at each state
    for (int i = 0; i < 2; i++) {
        State next = next_state(state, actions[i]);
        // recursively calculate action utilities
        // action utilities are the utility we get from forcing each action now and continuing recursively
        // this is the expected utility of each action we take
        if (player == 0) {
            action_utilities[i] = cfr(next, p1_reach * strategy[i], p2_reach);
        } else {
            action_utilities[i] = cfr(next, p1_reach, p2_reach * strategy[i]);
        }
    }
    // now calculate the whole EV for the node
    // odds of each action * utility of each action
    double node_utility = strategy[0] * action_utilities[0] + strategy[1] * action_utilities[1];
    // assign reach probabilities
    double opponent_reach = (player == 0) ? p2_reach : p1_reach;
    double self_reach = (player == 0) ? p1_reach : p2_reach;

    // Now that we have analyzed every action under this node, we can calculate regret
    // for each action
    for (int i = 0; i < 2; i++) {
        double regret;
        // if it was our turn to move
        // our regret is the utility we got from taking said action minus the utility we already had in the node
        // remember, higher positive regret means we regret that we DIDN'T take this action
        if (player == 0) {
            regret = action_utilities[i] - node_utility;
        // flip signs on regret if it was the opponents turn
        // our loss is their gain and vice versa
        // everything is being calculated from our perspective here
        } else {
            regret = node_utility - action_utilities[i];
        }
        // We can't treat every node as the same, some happen more than others
        // so based on when our opponent makes this spot relevant, we should learn regret based on that decision
        // That tells us how much this local mistake matters in the full game.
        node.regret_sum[i] += opponent_reach * regret;
        // Strategy sum instead tracks how often we actually used this decision
        // so we have to modify our strategy learning based on how often WE tend to land in this node.
        node.strategy_sum[i] += self_reach * strategy[i];
    }

    return node_utility;
}

void CFRTrainer::train(int iterations) {
        std::vector<Card> cards{Card::J, Card::Q, Card::K};

        for (int iter = 0; iter < iterations; iter++) {
            for (Card p1_card : cards) {
                for (Card p2_card : cards) {
                    if (p1_card == p2_card) {
                        continue;
                    }
                    State root{p1_card, p2_card, ""};
                    cfr(root, 1.0, 1.0);
                }
            }
        }
}

void CFRTrainer::print_strategies() const {
    std::vector<std::string> keys;
    keys.reserve(node_map_.size());

    for (const auto& entry : node_map_) {
        keys.push_back(entry.first);
    }

    std::sort(keys.begin(), keys.end());

    for (const std::string& key : keys) {
        const Node& node = node_map_.at(key);
        std::array<double, 2> avg = node.get_average_strategy();
        std::cout << key
                 << "-> ["
                 << avg[0]
                 << ", "
                 << avg[1]
                 << "]\n";
    }
}

}