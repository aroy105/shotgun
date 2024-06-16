from cfr.trainer import CFRTrainer

def main():
    trainer = CFRTrainer()
    trainer.train(100_000)
    avg_strategies = trainer.get_average_strategy()
    for info_set, strategy in avg_strategies.items():
        print(info_set, strategy)