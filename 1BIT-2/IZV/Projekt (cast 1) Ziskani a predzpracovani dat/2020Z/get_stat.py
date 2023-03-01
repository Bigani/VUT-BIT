from download import DataDownloader
import matplotlib
import sys
import os
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
matplotlib.use('TkAgg')

PHA = ("00", "PHA")
STC = ("01", "STC")
JHC = ("02", "JHC")
PLK = ("03", "PLK")
ULK = ("04", "ULK")
HKK = ("05", "HKK")
JHM = ("06", "JHM")
MSK = ("07", "MSK")
OLK = ("14", "OLK")
ZLK = ("15", "ZLK")
VYS = ("16", "VYS")
PAK = ("17", "PAK")
LBK = ("18", "LBK")
KVK = ("19", "KVK")

# Input variables
# # # # # # # # #
INPUT_REGIONS = [LBK, OLK, ULK, PHA]
# # # # # # # # #


# zpracuje predošle sparsované data do grafou
def plot_stat(data_source, fig_location=None, show_figure=True):
    bar_label = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14]

    fig, ax = plt.subplots(5, sharey='all')
    fig.set_figheight(8)
    fig.set_figwidth(13)

    # rozdelit podla rokov
    for i in range(5):
        year = i + 2016

        year_list = [x for x in data_source[1] if x[3].year == year]  # list s udajmi z daneho roku
        reg, num = count_by_regions(year_list)

        bars = ax[i].bar(reg, num, width=0.4, bottom=0, align="center", color='C3')  # setup
        # Očíslenie jednotlivých stĺpcov
        for idx, bar in enumerate(bars):
            height = bar.get_height()
            ax[i].text(idx, height * 1.05, bar_label[idx], ha='center')
        ax[i].grid(axis="y")  # grid na osi y
        if year == 2020:  # specialna podmienak
            ax[i].set_title("2020 Září")
        else:
            ax[i].set_title(year)
        ax[i].spines['top'].set_visible(False)
        ax[i].spines['right'].set_visible(False)
        ax[i].yaxis.set_major_locator(ticker.MultipleLocator(5000))
    plt.tight_layout()

    if fig_location is not None:
        # dummy dumb file saving directory solution
        head, tail = os.path.split(fig_location)
        if not head.startswith(".") and head.startswith("/"):  # pri zadavani fig location ak nahodou
            head = "." + head
        elif not head.startswith(".") and not head.startswith("/"):  # pri zadavani fig location ak nahodou
            head = "./" + head
        DataDownloader.makedir(data_source, head)
        try:
            plt.savefig(head + "/" + tail, bbox_inches='tight')
        except PermissionError:
            print("Insufficient permission")
    if show_figure:
        plt.show()


# par tuple return dva listy s usporiadanymi daty podla regionov
def count_by_regions(data):
    states = []
    counts = []
    ccount = 0
    for each in data:

        if each[len(each) - 1] in states:  # same state
            ccount += 1
        else:  # new state
            states.append(each[len(each) - 1])
            if ccount != 0:  # not if first
                counts.append(ccount)
            ccount = 1

    counts.append(ccount)  # last
    # # # # # #
    # zoradenie podla mnozstva nehod
    regnum = {}
    for num in counts:
        for reg in states:
            regnum[num] = reg
            states.remove(reg)
            break

    lists = sorted(regnum.items(), reverse=True)

    y, x = zip(*lists)
    return x, y


def main(argv):
    fig_loc = None
    fig_show = False

    if len(argv) > 3:
        print("Incorrect arguments")

    if "--fig_location" in argv:
        try:
            if argv[1] != "--show_figure":
                fig_loc = argv[1]
            else:
                print("Argument Error missing fig location par")
                exit(1)
        except IndexError:
            print("Argument Error missing fig location par")
            exit(1)
    if "--show_figure" in argv:
        fig_show = True

    data_source = DataDownloader().get_list()
    plot_stat(data_source, fig_loc, fig_show)


if __name__ == "__main__":
    main(sys.argv[1:])

# 1:23 vsetko #
# 0:08 cache  #