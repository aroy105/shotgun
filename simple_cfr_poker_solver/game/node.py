class Node:
    def __init__(self, info_set):
        self.info_set = info_set
        self.regret_sum = {action: 0 for action in ["pass", "bet"]}
        self.strategy = {action: 0.5 for action in ["pass", "bet"]}
        self.strategy_sum = {action: 0 for action in ["pass", "bet"]}
    
    def get_strategy(self, realization_weight):
        normalizing_sum = 0
        for action in self.regret_sum:
            self.strategy[action] = max(0, self.regret_sum[action])
            normalizing_sum += self.strategy[action]
        
        for action in self.strategy:
            if normalizing_sum > 0:
                self.strategy[action] /= normalizing_sum
            else:
                self.strategy[action] = 0.5
            
            self.strategy_sum[action] += realization_weight * self.strategy[action]
        
        return self.strategy