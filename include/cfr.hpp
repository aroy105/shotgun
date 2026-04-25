#pragma once

#include <string>
#include <unordered_map>

#include "kuhn.hpp"

namespace shotgun::kuhn {
class CFRTrainer {
public:
    double cfr(const State& state, double p1_reach, double p2_reach);
    void train(int iterations);
    void print_strategies() const;

private:
    std::unordered_map<std::string, Node> node_map_;
};
}