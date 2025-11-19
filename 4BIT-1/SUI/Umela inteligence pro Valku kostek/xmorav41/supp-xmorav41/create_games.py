
#!/usr/bin/env python3

import random
import sys
from argparse import ArgumentParser
import time
from signal import signal, SIGCHLD
from scripts.utils  import run_ai_only_game, BoardDefinition
import dicewars.shared as sh
parser = ArgumentParser(prog='Dice_Wars')
parser.add_argument('-p', '--port', help="Server port", type=int, default=5005)
parser.add_argument('-a', '--address', help="Server address", default='127.0.0.1')
#parser.add_argument('-l', '--logs', help="logs", default='logs/')

procs = []


def signal_handler(signum, frame):
    """ Handler for SIGCHLD signal that terminates server and clients. """
    for p in procs:
        try:
            p.kill()
        except ProcessLookupError:
            pass

AIs = [
   # 'xlogin42_2',
    'xdubec01',
    #'kb.sdc_post_at',
    #'kb.sdc_post_dt',
    'kb.sdc_pre_at',
    'kb.stei_adt',
    'kb.stei_at',
    'kb.stei_dt',
    #'dt.sdc',
    #'dt.ste',
    'dt.stei',
    #'dt.wpm_c',
    #'dt.wpm_d',
    #'dt.wpm_s',
]


def board_definitions():
    while True:
        random.seed(int(time.time()))
        yield BoardDefinition(random.randint(1, 10 ** 10), random.randint(1, 10 ** 10), random.randint(1, 10 ** 10))


def main():
    args = parser.parse_args()

    signal(SIGCHLD, signal_handler)

    played = 0
    try:
        for board_definition in board_definitions():
            played += 1
            players = random.sample(AIs,4)
            print("Players for next round:", players)
            run_ai_only_game(
                args.port, args.address, procs, players,
                board_definition,
                fixed=random.randint(1, 10 ** 10),
                client_seed=random.randint(1, 10 ** 10), debug=True, logdir='logs'
            )
            print(f'Played {played} games.', file=sys.stderr)
            #break;
    except (Exception, KeyboardInterrupt) as e:
        sys.stderr.write("Breaking the tournament because of {}\n".format(repr(e)))
        raise


if __name__ == '__main__':
    main()