from dicewars.ai.utils import probability_of_holding_area, probability_of_successful_attack,possible_attacks,probability_of_successful_attack
import numpy as np
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
def serialized_game(players_order, board):
    game = []
    features = np.empty((0, 32), dtype=int)

    for i in players_order:

        game_tmp = [
            board.get_player_dice(i), #pocet kociek jednotlivych hracou
            max(len(region) for region in board.get_players_regions(i)), # velkost najvacsieho regionu jednotlivych hracov
            len(board.get_players_regions(i)), # pocet regionov jednotlivych hracov
            len(board.get_player_areas(i)), # pocet uzemi jednotlivych hracov
            len([area for area in board.get_player_areas(i) if area.get_dice() >= 4]),
            len([area for area in board.get_player_areas(i) if area.get_dice() < 4]),
            easy_targets(board,i),
            get_weak_lands(board,i)
        ]
        for val in game_tmp:
            game.append(val)
    features = np.concatenate(
        ([game],features),
    )
    return features

def areas_expected_loss(board, player_name, areas):
    hold_ps = [probability_of_holding_area(board, a.get_name(), a.get_dice(), player_name) for a in areas]
    return sum((1 - p) * a.get_dice() for p, a in zip(hold_ps, areas))
def get_transfer_to_border(board, player_name):
    border_names = [a.name for a in board.get_player_border(player_name)]
    all_areas = board.get_player_areas(player_name)
    inner = [a for a in all_areas if a.name not in border_names]

    for area in inner:
        if area.get_dice() < 2:
            continue

        for neigh in area.get_adjacent_areas_names():
            if neigh in border_names and board.get_area(neigh).get_dice() < 8:
                return area.get_name(), neigh

    return None
def get_transfer_from_endangered(board, player_name):
    border_names = [a.name for a in board.get_player_border(player_name)]
    all_areas_names = [a.name for a in board.get_player_areas(player_name)]

    retreats = []

    for area in border_names:
        area = board.get_area(area)
        if area.get_dice() < 2:
            continue

        for neigh in area.get_adjacent_areas_names():
            if neigh not in all_areas_names:
                continue
            neigh_area = board.get_area(neigh)

            expected_loss_no_evac = areas_expected_loss(board, player_name, [area, neigh_area])

            src_dice = area.get_dice()
            dst_dice = neigh_area.get_dice()

            dice_moved = min(8 - dst_dice, src_dice - 1)

            area.dice -= dice_moved
            neigh_area.dice += dice_moved

            expected_loss_evac = areas_expected_loss(board, player_name, [area, neigh_area])

            area.set_dice(src_dice)
            neigh_area.set_dice(dst_dice)

            retreats.append(((area, neigh_area), expected_loss_no_evac - expected_loss_evac))

    retreats = sorted(retreats, key=lambda x: x[1], reverse=True)

    if retreats:
        retreat = retreats[0]
        if retreat[1] > 0.0:
            return retreat[0][0].get_name(), retreat[0][1].get_name()

    return None