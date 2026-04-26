#include "cfr.hpp"

#include <algorithm>
#include <array>
#include <iomanip>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

namespace shotgun::kuhn {

// private 
// will store the temporary updates for one iteration of CFR
// batch updating - computing all updates first before applying them
namespace {
struct NodeDelta {
    std::array<double, 2> regret_sum{};
    std::array<double, 2> strategy_sum{};
};
// infoset_key -> temp infoset updates
// i.e. "K|b" -> NodeDelta
using IterationDeltas = std::unordered_map<std::string, NodeDelta>;
constexpr double kChanceReach = 1.0 / 6.0;

// wrapper for existing terminal_utility function
// returns terminal utility from current player's pov
double terminal_utility_for_current_player(const State& state) {
    // utility from player 1's perspective (existing function)
    double player_one_utility = terminal_utility(state);
    // based on history length determine current player
    int terminal_player = static_cast<int>(state.history.size() % 2);
    // flip utility if it is player 2
    return terminal_player == 0 ? player_one_utility : -player_one_utility;
}

// after one iteration of CFR, apply the new regrets and strategies
void apply_deltas(std::unordered_map<std::string, Node>& node_map, const IterationDeltas& deltas) {
    // go through every changed infoset this iteration
    for (const auto& [key, delta] : deltas) {
        // get the actual infoset (or create it if not made yet)
        Node& node = node_map[key];
        // for both actions, update regret_sum and strategy_sum
        for (int i = 0; i < 2; i++) {
            node.regret_sum[i] += delta.regret_sum[i];
            node.strategy_sum[i] += delta.strategy_sum[i];
        }
    }
}

double cfr_impl(
    const State& state,
    // the odds of player 1's strategy got us here
    double p1_reach,
    // the odds of player 2's strategy got us here
    double p2_reach,
    // odds of this particular deal of the cards
    double chance_reach,
    std::unordered_map<std::string, Node>& node_map,
    IterationDeltas& deltas
) {
    // once we recursively reached a terminal node (showdown or fold) return utility
    // remember, reach probabilities are our odds of getting to some state
    // If player 1 bets at root w p=0.4, and we call with p=0.3 facing a bet
    // the reach probability of history "bc" due to strategic choices is p1_reach = 0.4, p2_reach = 0.3
    // and strategic reach is 0.12
    if (is_terminal(state)) {
        return terminal_utility_for_current_player(state);
    }
    // from our state we will derive our infoset
    std::string key = infoset_key(state);
    // node represents the infoset, like K|cb or J|, where we only know our card and actions taken
    // if no node exists, C++ creates a default one
    // default means regret_sum = strategy_sum = 0, 0
    Node& node = node_map[key]; 
    // get the current strategy from the regrets, our actions, and init empty action utilities array
    // initially prior to any traversal of the tree, regret_sum and strategy_sum are initialized to 0, 0
    // and in our get_strategy code, if regret = 0, 0
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
        // we use negative returned utility. Negamax style since we are getting the other player's utility.
        if (player == 0) {
            action_utilities[i] = -cfr_impl(
                next,
                p1_reach * strategy[i], // player 1 action we just took now gets us to next recursive step
                p2_reach,
                chance_reach,
                node_map,
                deltas
            );
        } else {
            action_utilities[i] = -cfr_impl(
                next,
                p1_reach,
                p2_reach * strategy[i], // player 2 action we just took now gets us to next recursive step
                chance_reach,
                node_map,
                deltas
            );
        }
    }
    // now calculate the whole EV for the node
    // odds of each action * utility of each action
    double node_utility = strategy[0] * action_utilities[0] + strategy[1] * action_utilities[1];
    // assign reach probabilities
    double opponent_reach = (player == 0) ? p2_reach : p1_reach;
    double self_reach = (player == 0) ? p1_reach : p2_reach;

    // Now that we have analyzed every action under this node, and the actions from nodes below, we can calculate regret
    // for each action
    for (int i = 0; i < 2; i++) {
        // Our regret is the utility from forcing this action minus the utility
        // from following the current mixed strategy at this node.
        // remember, higher positive regret means we regret that we DIDN'T take this action
        double regret = action_utilities[i] - node_utility;
        // update the temporary regrets and strategy, so first find the delta we iterated on
        NodeDelta& delta = deltas[key];
        // We can't treat every node as the same, some happen more than others
        // so based on when our opponent makes this spot relevant, we should learn regret based on that decision
        // That tells us how much this local mistake matters in the full game.
        // each card deal (chance reach) happens 1/6 times, regret is counterfactual so assuming I got here (i.e. only consider odds of opponent getting here)
        // then from these two probabilities, scale regret
        delta.regret_sum[i] += chance_reach * opponent_reach * regret;
        // Strategy sum instead tracks how often we actually used this decision
        // so we have to modify our strategy learning based on how often WE tend to land in this node and make this decision
        // so our strategy should scale with odds of this card deal and odds we land in this spot
        delta.strategy_sum[i] += chance_reach * self_reach * strategy[i];
    }
    // return the value of this node from current player's perspective
    return node_utility;
}

// helper to get average strategy for a node from node_map and key
std::array<double, 2> average_strategy_for(
    const std::unordered_map<std::string, Node>& node_map,
    const std::string& key
) {
    // if node exists, return the average strategy for the node
    auto iter = node_map.find(key);
    if (iter != node_map.end()) {
        return iter->second.get_average_strategy();
    }
    // otherwise, node wasn't visited, so return average strategy (50/50)
    Node unvisited;
    return unvisited.get_average_strategy();
}

// recursively evalute learned average strategy, not the current regret matched one
double expected_value_impl(
    const State& state,
    const std::unordered_map<std::string, Node>& node_map
) {
    // if terminal, utility is just terminal utility
    if (is_terminal(state)) {
        return terminal_utility(state);
    }
    
    // calculate current strategy policy
    std::string key = infoset_key(state);
    std::array<double, 2> strategy = average_strategy_for(node_map, key);
    std::vector<Action> actions = legal_actions(state);

    // across each strategy, calculate expected value of both actions and add them
    double value = 0.0;
    for (int i = 0; i < 2; i++) {
        // remember strategy[i] is odds we take a certain action
        value += strategy[i] * expected_value_impl(next_state(state, actions[i]), node_map);
    }
    // return the summed EV of both actions
    return value;
}
} // end of private 

// given a given state and existing reach probabilities, this runs CFR from that state
// not used in actual trainer, better for testing to confirm functions work
double CFRTrainer::cfr(const State& state, double p1_reach, double p2_reach) {
    IterationDeltas deltas;
    double utility = cfr_impl(state, p1_reach, p2_reach, 1.0, node_map_, deltas);
    apply_deltas(node_map_, deltas);

    return utility;
}

// actual trainer class
void CFRTrainer::train(int iterations) {
        std::vector<Card> cards{Card::J, Card::Q, Card::K};
        // for each iteration
        for (int iter = 0; iter < iterations; iter++) {
            IterationDeltas deltas;
            // go through all possible deals
            for (Card p1_card : cards) {
                for (Card p2_card : cards) {
                    if (p1_card == p2_card) {
                        continue;
                    }
                    // run CFR on the deal
                    State root{p1_card, p2_card, ""};
                    cfr_impl(root, 1.0, 1.0, kChanceReach, node_map_, deltas);
                }
            }
            // now that we have finshed an iteration, batch update new regrets and strategies across all possible deals
            apply_deltas(node_map_, deltas);
        }
}

// getter for average_strategy_for
std::array<double, 2> CFRTrainer::average_strategy(const std::string& key) const {
    return average_strategy_for(node_map_, key);
}

// calculate EV of each learned average strategy 
double CFRTrainer::expected_value() const {
    std::vector<Card> cards{Card::J, Card::Q, Card::K};
    double value = 0.0;

    for (Card p1_card : cards) {
        for (Card p2_card : cards) {
            if (p1_card == p2_card) {
                continue;
            }

            value += kChanceReach * expected_value_impl(State{p1_card, p2_card, ""}, node_map_);
        }
    }

    return value;
}

void CFRTrainer::print_strategies() const {
    std::vector<std::string> keys;
    keys.reserve(node_map_.size());

    for (const auto& entry : node_map_) {
        keys.push_back(entry.first);
    }

    std::sort(keys.begin(), keys.end());

    std::cout << std::fixed << std::setprecision(4);
    std::cout << "Average strategy:\n";

    for (const std::string& key : keys) {
        const Node& node = node_map_.at(key);
        std::array<double, 2> avg = node.get_average_strategy();
        std::size_t separator = key.find('|');
        std::string history = key.substr(separator + 1);
        State dummy_state{Card::J, Card::Q, history};
        std::vector<Action> actions = legal_actions(dummy_state);
        std::cout << key << " -> ";

        for (int i = 0; i < 2; i++) {
            std::cout << action_to_string(actions[i]) << ": " << avg[i];
            if (i < 1) {
                std::cout << " | ";
            }
        }

        std::cout << "\n";
    }
}

}
