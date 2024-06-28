def test_available_actions():
    from game.kuhn_poker import available_actions
    
    assert "pass" in available_actions(""), "Should have pass option"
    assert "bet" in available_actions(""), "Should have bet option"