import random

def deal_cards(deck):
    shuffled = deck.copy()
    random.shuffle(shuffled)
    return shuffled[:2]