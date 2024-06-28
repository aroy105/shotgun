from game.node import Node

from game.kuhn_poker import is_terminal, payoff, available_actions, build_info_set, get_current_player
from utils.deck import deal_cards

class CFRTrainer:
    def __init__(self):
        self.node_map = {}
        self.cards = ["J", "Q", "K"]

    def train(self, iterations):
        for _ in range(iterations):
            cards = deal_cards(self.cards)
            self.cards_p0 = cards[0]
            self.cards_p1 = cards[1]
            self.cfr("", 1, 1)

    def cfr(self, history, prob_p0, prob_p1):
        if is_terminal(history):
            return payoff(history, self.cards_p0, self.cards_p1)

        player = get_current_player(history)
        player_card = self.cards_p0 if player == 0 else self.cards_p1

        info_set = build_info_set(player_card, history)

        if info_set not in self.node_map:
            self.node_map[info_set] = Node(info_set)

        node = self.node_map[info_set]
        strategy = node.get_strategy(prob_p0 if player == 0 else prob_p1)

        util = {}
        node_util = 0

        for action in available_actions(history):
            next_history = history + action[0]
            if player == 0:
                util[action] = -self.cfr(next_history, prob_p0 * strategy[action], prob_p1)
            else:
                util[action] = -self.cfr(next_history, prob_p0, prob_p1 * strategy[action])

            node_util += strategy[action] * util[action]

        for action in available_actions(history):
            regret = util[action] - node_util
            if player == 0:
                node.regret_sum[action] += prob_p1 * regret
            else:
                node.regret_sum[action] += prob_p0 * regret

        return node_util

    def get_average_strategy(self):
        avg_strategies = {}
        for info_set, node in self.node_map.items():
            avg_strategies[info_set] = node.get_average_strategy()
        return avg_strategies
