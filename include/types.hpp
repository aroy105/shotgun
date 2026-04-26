#pragma once

#include <algorithm>
#include <array>
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

struct Node {
    std::array<double, 2> regret_sum{};
    std::array<double, 2> strategy_sum{};

    std::array<double, 2> get_strategy() const {
        std::array<double, 2> strategy{
            std::max(0.0, regret_sum[0]),
            std::max(0.0, regret_sum[1]),
        };

        double normalizing_sum = strategy[0] + strategy[1];

        if (normalizing_sum > 0.0) {
            strategy[0] /= normalizing_sum;
            strategy[1] /= normalizing_sum;
        } else {
            strategy[0] = 0.5;
            strategy[1] = 0.5;
        }

        return strategy;
    }

    std::array<double, 2> get_average_strategy() const {
        std::array<double, 2> avg{
            strategy_sum[0],
            strategy_sum[1]
        };

        double normalizing_sum = avg[0] + avg[1];

        if (normalizing_sum > 0.0) {
            avg[0] /= normalizing_sum;
            avg[1] /= normalizing_sum;
        } else {
            avg[0] = 0.5;
            avg[1] = 0.5;
        }

        return avg;
    }
};

}