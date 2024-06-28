def test_trainer_runs():
    from cfr.trainer import CFRTrainer
    
    trainer = CFRTrainer()
    trainer.train(100)
    
    assert len(trainer.node_map) > 0, "Trainer should create nodes"
