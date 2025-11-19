import logging
import random
import torch
import numpy
import itertools
from dicewars.ai.utils import *
from .utils import *
from copy import deepcopy
from dicewars.client.ai_driver import BattleCommand, EndTurnCommand, TransferCommand
import dicewars.shared as sh


class FinalAI:
    """Naive player agent

    This agent performs all possible moves in random order
    """

    def __init__(self, player_name, board, players_order, max_transfers):
        """
        Parameters
        ----------
        game : Game
        """
        self.waiting_attack=[]
        self.board = board
        self.max_tran = max_transfers
        self.player_name = player_name
        self.reserved_evacs = max_transfers // 3
        self.players_order = players_order
        self.logger = logging.getLogger('AI')
        self.model = torch.load('dicewars/ai/xmorav41/model.pth', map_location=torch.device('cpu'))
    def end(self, player):
        sh.dump_data(player,self)
    def ai_turn(self, board, nb_moves_this_turn, nb_transfers_this_turn, nb_turns_this_game, time_left):
        """AI agent's turn

        Get a random area. If it has a possible move, the agent will do it.
        If there are no more moves, the agent ends its turn.
        """
        while self.players_order[0] != self.player_name:
            self.players_order = [self.players_order[(i + 3) % len(self.players_order)]
                                  for i, x in enumerate(self.players_order)]
        stav = self.calc_state(self.players_order, board)

        if nb_transfers_this_turn + self.reserved_evacs < self.max_tran:
            transfer = get_transfer_to_border(board, self.player_name)
            if transfer:
                return TransferCommand(transfer[0], transfer[1])
        turns = self.order_attacks(board, self.player_name)
        state = self.calc_state(self.players_order, board)
        turn = 0, 0
        '''
        for a, d in turns:
            copy = deepcopy(board)
            self.perform_attack(copy, a, d)
            state_n = self.calc_state(self.players_order, copy)
            if state_n > state: #if state_n[self.player_name] > state:
                turn = a, d
                state = state_n
        self.logger.debug("Possible novy stav {0} {1}".format(state,turn))
        if (turn != (0,0)):
            a, b = turn
            return BattleCommand(a, b)
        '''
        #
        #for at in self.waiting_attack:
            #a,b = at
            #self.waiting_attack.pop()
            #return BattleCommand(a, b)
        turns, h = self.maxn_calc(board, self.players_order + self.players_order, self.players_order, 1)
        self.logger.debug("Possible novy stav {0} {1}".format(h, turns))
        if not (turns == None or len(turns) == 0) :
            a,b = turns[0]
            turns.pop()
            self.waiting_attack = turns
            return BattleCommand(a,b)




        if nb_transfers_this_turn < self.max_tran:
                transfer = get_transfer_from_endangered(board, self.player_name)
                if transfer:
                    return TransferCommand(transfer[0], transfer[1])
        return EndTurnCommand()

    def order_attacks(self, board_copy, player ):
        all_moves = list(possible_attacks(board_copy, player))
        turns = []
        for a, d in all_moves:
            attack = a.get_dice()

            p = probability_of_successful_attack(board_copy, a.get_name(), d.get_name())
            p *= probability_of_holding_area(board_copy, d.get_name(), attack - 1, player)
            if p >= 0.4 or attack == 8:
                if a.get_name() in self.get_largest_region(player, board_copy):
                     p *= 2
                turns.append((a.get_name(), d.get_name(), p))
        turns = sorted(turns, key=lambda t: t[2], reverse=True)

        moves = []
        for a, b, c in turns:
            moves.append((a, b))
        return moves

    def create_ways(self, turns):
        ways=[]

        for a,d in turns:
            for i in range(0,len(turns)):
                ways.append([(a,d)])
            for way in ways:
                found = False
                for b,c in way:
                    if a==b:
                        found = True
                    if d == c:
                        found = True
                if not found:
                    if way + [(a,d)] not in ways:
                        way.append((a,d))
        ways.sort()
        ways =  list(k for k, _ in itertools.groupby(ways))
        return  ways
    def maxn_calc(self, board, players,order, width):

        if len(players) == 0:
            self.logger.debug("Tady")
            return None, self.evaluate_state(order, board)
        self.logger.debug(players)
        flag = False
        if(len(players) == 8):
            flag = True
        player = players[0]
        copy = deepcopy(board)
        if width == 1:
            # ray spravis to border simulaciu
            for i in range(0,3):
                if flag:
                    continue
                transfer = get_transfer_to_border(copy,player)
                if transfer:
                    self.perform_transfer(copy, transfer[0], transfer[1])
                else:
                    break
            board = copy
            ordered_turns = self.order_attacks(board, player)
            if not ordered_turns:
                if flag:
                    players=[]
                self.logger.debug("Tu.{0}".format(player))
                turns, h = self.maxn_calc(board, players, order, 0)
                return None, h
            state = {}
            turns = []
            ret_turns=[]
            ways = self.create_ways(ordered_turns)
            self.logger.debug(ways)
            if(flag and len(ways) == 1):
                return ways[0],0
            for way in ways:
                self.logger.debug("Tady.{0} cesta{1}".format(player,way))
                copy = deepcopy(board)
                for a,d in way:
                    self.perform_attack(copy, a, d)
                    turns.append((a, d))
                _, h = self.maxn_calc(copy, players, order, 0)

                if player in h:
                    if player not in state or h[player] > state[player]:
                        state = h
                        self.logger.debug("Hrac {0}a stav{1}".format(player,state))
                        self.logger.debug("Cesta{0}".format(turns))
                        ret_turns = turns
                turns=[]
            return ret_turns, state

        players.pop(0)
        if (len(players) >= 4):
            order = players[:4]
        else:
            order = order[1:] + [order[0]]
        if len(players) != 0:
            return self.maxn_calc(
                deepcopy(board), players, order, 1
            )
        self.logger.debug("Ahoj")
        return None, self.evaluate_state(order, board)
    def perform_attack(self, board, attack, defense):
        attack = board.get_area(attack)
        defense = board.get_area(defense)

        defense.set_dice(attack.get_dice() - 1)
        attack.set_dice(1)
        defense.set_owner(attack.get_owner_name())
    def perform_transfer(self, board, source, destination):
        source = board.get_area(source)
        destination = board.get_area(destination)
        val = source.get_dice() + destination.get_dice() - 1
        source.set_dice(1)
        destination.set_dice(8 if val > 8 else  val)
    def evaluate_state(self, order,board):
        ret_dict = {}
        for i in range(0,4):
            ret_dict.update({order[0] : self.calc_state(order,board)})
            order = order[1:] + [order[0]]
        self.logger.debug("Tento dict sa zavola n razy.{0}".format(ret_dict))
        return ret_dict
    
    def calc_state(self, order, board):
        game = serialized_game(order,board)
        game = torch.from_numpy(game).float()
        temp=0.0
        part=0.0

        for i in range(0,6):
            p_to_be_in_certain_part_of_winning_game = self.model(game)[0,i].tolist()
            if (p_to_be_in_certain_part_of_winning_game > temp ) and (p_to_be_in_certain_part_of_winning_game >0.5):
                temp = p_to_be_in_certain_part_of_winning_game
                part= i/10 + 0.5 + (p_to_be_in_certain_part_of_winning_game/10)
        temp=part
        return temp
        
    def get_largest_region(self, player_name, board):
        
        """Get size of the largest region, including the areas within
        Attributes
        ----------
        largest_region : list of int
            Names of areas in the largest region
        Returns
        -------
        int
            Number of areas in the largest region
        """
        


        players_regions = board.get_players_regions(player_name)
        max_region_size = max(len(region) for region in players_regions)
        max_sized_regions = [region for region in players_regions if len(region) == max_region_size]


        return max_sized_regions[0]
