#pragma once

#include <string>
#include <vector>

#include "types.hpp"

namespace shotgun::kuhn {

bool is_terminal(const State& state);
std::vector<Action> legal_actions(const State& state);
State next_state(const State& state, Action action);
int current_player(const State& state);
int terminal_utility(const State& state);
std::string infoset_key(const State& state);
std::string action_to_string(Action action);

}