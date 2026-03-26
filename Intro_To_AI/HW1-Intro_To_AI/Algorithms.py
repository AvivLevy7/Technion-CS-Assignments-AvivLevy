import numpy as np
from HaifaEnv import HaifaEnv
from typing import List, Tuple
import heapdict
from collections import deque

class Node():
  def __init__(self,me,parent,act,cost,g=None,f=None) -> None:
    self.state = me 
    self.parent = parent 
    self.act = act 
    self.cost = cost
    self.g = g
    self.f = f

  def update(self, parent, act, g, f):
      self.parent = parent
      self.act = act
      self.g = g
      self.f = f


class Agent(): 
  def __init__(self) -> None: 
    self.env = None
    self.passway = 100

  def solution(self,end): 
    moves = [] 
    allcost = 0 
    while end.parent is not None:
      moves.append(end.act)
      allcost += end.cost 
      end = end.parent 
    return (moves[::-1],allcost)

  def calc_heuristic(self, state) -> int:
    r, c = self.env.to_row_col(state)
    goals_list = self.env.get_goal_states()
    heuristic_list = []
    for goal_state in goals_list:
        r1, c1 = self.env.to_row_col(goal_state)
        heuristic_list.append(abs(r1 - r) + abs(c1 - c))
    return min(min(heuristic_list), self.passway)


class BFSGAgent(Agent): 
  def __init__(self) -> None: 
    super().__init__()

  def search(self, env: HaifaEnv) -> Tuple[List[int], float, int]: 
    self.env = env 
    self.env.reset() 
    extended = 0 
    first = Node(self.env.get_initial_state(),None,-1,0) 
    if self.env.is_final_state(first.state): 
      moves,total = self.solution(first) 
      return (moves,total,extended) 
    pq = deque() 
    pq.append(first) 
    seen = set() 
    while pq: 
      cur = pq.popleft()
      extended += 1 
      seen.add(cur.state) 
      for act,(nex,cost,_) in self.env.succ(cur.state).items():
        if nex is None:
          continue 
        child = Node(nex,cur,act,cost) 
        if child.state not in seen:
          seen.add(child.state) 
          if self.env.is_final_state(child.state): 
            moves,total = self.solution(child) 
            return (moves,total,extended)
          pq.append(child) 
    return ([],-1,-1)


class GreedyAgent(Agent):
    def __init__(self) -> None:
        super().__init__()

    def search(self, env: HaifaEnv) -> Tuple[List[int], float, int]:
        self.env = env
        self.env.reset()
        extended = 0
        open_queue = heapdict.heapdict()
        close_set = set()
        start_state = self.env.get_initial_state()
        first_node = Node(start_state, None, -1, 0)
        open_queue[first_node] : Tuple[float, int] = (self.calc_heuristic(start_state), start_state)

        while open_queue:
            current_node, _ = open_queue.popitem()
            current_state = current_node.state
            close_set.add(current_state)
            if self.env.is_final_state(current_state):
                moves, total = self.solution(current_node)
                return (moves, total, extended)
            extended += 1
            for act, (new_state, cost, _) in self.env.succ(current_state).items():
                if new_state is None:
                    continue
                open_set = {t[1] for t in open_queue.values()}
                if (new_state not in close_set) and (new_state not in open_set):
                    child = Node(new_state, current_node, act, cost)
                    open_queue[child] = (self.calc_heuristic(new_state), new_state)
        return ([], -1, -1)


class AStarEpsilonAgent(Agent):
    def __init__(self):
        super().__init__()

    def search(self, env: HaifaEnv, epsilon: float = None):
        self.env = env
        self.env.reset()
        extended = 0
        open_queue = []
        close_set = set()
        start_state = self.env.get_initial_state()
        f_val = 0.5 * (self.calc_heuristic(start_state))
        open_queue.append(Node(start_state, None, -1, 0, 0, f_val))

        while open_queue:
            min_f = min({n.f for n in open_queue})
            focal_list = []
            k = 0
            for n in open_queue:
                if n.f <= (1 + epsilon) * min_f:
                    focal_list.append((n,k))
                    k += 1

            m = min([self.calc_heuristic(n[0].state) for n in focal_list])
            similarity_list = {n[0] : n[1] for n in focal_list if self.calc_heuristic(n[0].state) == m}
            current_node = min(similarity_list, key=similarity_list.get)
            current_state = current_node.state

            open_queue.remove(current_node)
            close_set.add(current_node)
            if self.env.is_final_state(current_state):
                moves, total = self.solution(current_node)
                return (moves, total, extended)

            extended += 1
            for act, (new_state, cost, _) in env.succ(current_state).items():
                if new_state is None:
                    continue
                new_g = current_node.g + cost
                new_f = 0.5 * (self.calc_heuristic(new_state) + new_g)
                new_node_open = next(filter(lambda x: x.state == new_state, open_queue), None)
                new_node_close = next(filter(lambda x: x.state == new_state, close_set), None)
                if (new_node_open is None) and (new_node_close is None):
                    child = Node(new_state, current_node, act, cost, new_g, new_f)
                    open_queue.append(child)
                elif new_node_close is None:
                    if new_f < new_node_open.f:
                        new_node_open.update(current_node, act, new_g, new_f)
                else:
                    if new_f < new_node_close.f:
                        close_set.remove(new_node_close)
                        new_node_close.update(current_node, act, new_g, new_f)
                        open_queue.append(new_node_close)
        return ([], -1, -1)


class AStarAgent():
    def __init__(self):
        pass

    def search(self, env: HaifaEnv) -> Tuple[List[int], float, int]:
        a_search_e = AStarEpsilonAgent()
        return a_search_e.search(env, 0)
