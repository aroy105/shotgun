#include <iostream>

#include "cfr.hpp"


int main() {
    shotgun::kuhn::CFRTrainer trainer;

    trainer.train(10000);
    trainer.print_strategies();
    
    return 0;
}