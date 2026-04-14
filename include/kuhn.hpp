#pragma once

#include <string>

bool is_terminal(const std::string& history);
int terminal_utility(const std::string& history, char p1_card, char p2_card);