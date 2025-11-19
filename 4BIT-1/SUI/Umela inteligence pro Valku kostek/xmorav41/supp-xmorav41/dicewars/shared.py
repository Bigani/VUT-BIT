import numpy as np
import os
from dicewars.ai.utils import probability_of_holding_area, probability_of_successful_attack,possible_attacks,probability_of_successful_attack
DATA_DIR =  "data"
DATASET = 'sample.csv'
TRAIN_FILE = 'train.csv'
VALIDATE_FILE = 'validate.csv'
TEST_FILE = 'test.csv'
haha= 1
features = np.empty((0,33), dtype=float)
def dump_data(p,ai):
    global features
    global haha
    number_of_my_turns, columns = features.shape
    
    ai.logger.debug(columns)
    if(haha==0):
        return
    for cnt in range(0,number_of_my_turns):
        
        if(features[cnt,-1] == float(p)):
            features[cnt,-1] =  round(0.5 + 0.5*(float(cnt)/number_of_my_turns), 1)

        else:
            features[cnt,-1] = 0.0

    if not os.path.exists(DATASET):
         os.mknod(DATASET)
    
    with open(DATASET,'a') as csvfile:        
        np.savetxt(csvfile,features,delimiter=',',fmt='%.1f', comments='')
    
    haha=0
def get_weak_lands(bot_board, player_name):
    """get number of weak areas to enemies
    Attributes
    ----------
    bot_board : board
    player_name : int
    Returns
    -------
    int
        Number of weak areas
    """
    weak_areas = 0

    for area in bot_board.get_player_border(player_name):
        if probability_of_holding_area(bot_board, area.get_name(),area.get_dice(),player_name) < 0.3:
            weak_areas = weak_areas + 1
    return weak_areas


def easy_targets(bot_board, player_name):
    """get number of weak areas to enemies
    Attributes
    ----------
    bot_board : board
    player_name : int
    Returns
    -------
    int
        Number of weak areas
    """
    targets = 0

    attacks = possible_attacks(bot_board, player_name)
    for source, target in attacks:
        if(probability_of_successful_attack(bot_board,source.get_name(), target.get_name())) > 0.7:
            targets = targets + 1

    return targets

def extract_features(ai, board):
    global features
    l = []

    for i in ai.players_order:

        ai.logger.debug(features)
        xd = [
            board.get_player_dice(i), #pocet kociek jednotlivych hracou
            max(len(region) for region in board.get_players_regions(i)), # velkost najvacsieho regionu jednotlivych hracov
            len(board.get_players_regions(i)), # pocet regionov jednotlivych hracov
            len(board.get_player_areas(i)), # pocet uzemi jednotlivych hracov
            len([area for area in board.get_player_areas(i) if area.get_dice() >= 4]),
            len([area for area in board.get_player_areas(i) if area.get_dice() < 4]),
            easy_targets(board,i),
            get_weak_lands(board,i)
        ]
        for x in xd:
            l.append(x)
    l.append(ai.player_name)
    l = [l]
    ai.logger.debug("Tu")
    ai.logger.debug(l)
    ai.logger.debug(features)
    features = np.concatenate(
        (l, features),
    )
    ai.logger.debug(features)
