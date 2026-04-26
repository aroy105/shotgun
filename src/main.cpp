#include <iostream>

#include "cfr.hpp"


int main() {
    shotgun::kuhn::CFRTrainer trainer;

    trainer.train(10000);
    trainer.print_strategies();

    const auto j_root = trainer.average_strategy("J|");
    const auto k_root = trainer.average_strategy("K|");
    const auto q_facing_bet = trainer.average_strategy("Q|b");
    const auto q_check_bet = trainer.average_strategy("Q|cb");
    const double alpha = j_root[1];

    std::cout << "Expected value for player 1: "
              << trainer.expected_value()
              << " (target: -0.0556)\n";
    std::cout << "Nash family check:\n";
    std::cout << "alpha = J| Bet: " << alpha << "\n";
    std::cout << "K| Bet: " << k_root[1]
              << " (target: 3 * alpha = " << 3.0 * alpha << ")\n";
    std::cout << "Q|b Call: " << q_facing_bet[0]
              << " (target: 0.3333)\n";
    std::cout << "Q|cb Call: " << q_check_bet[0]
              << " (target: alpha + 0.3333 = " << alpha + (1.0 / 3.0) << ")\n";

    return 0;
}
