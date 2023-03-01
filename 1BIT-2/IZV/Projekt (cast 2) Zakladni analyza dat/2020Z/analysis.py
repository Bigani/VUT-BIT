#!/usr/bin/env python3.8
# coding=utf-8

from matplotlib import pyplot as plt
import pandas as pd
import seaborn as sns
import numpy as np

# muzete pridat libovolnou zakladni knihovnu ci knihovnu predstavenou na prednaskach
# dalsi knihovny pak na dotaz

MiB = 1048576


# Ukol 1: nacteni dat
def get_dataframe(filename: str, verbose: bool = False) -> pd.DataFrame:
    df = pd.read_pickle(filename, compression='gzip')
    df['date'] = pd.to_datetime(df['p2a'])
    if verbose:
        print("orig_size={:.1f} MiB".format(df.memory_usage(deep=True).sum() / 1048576))

    for col in list(df):
        if col not in ['d', 'e', 'p1', 'p12', 'p13a', 'p13b', 'p13c', 'p14', 'p2a', 'p2b', 'p33d', 'p33e', 'p34',
                       'p47', 'p5b', 'region']:
           df[col] = df[col].astype("category")

    if verbose:
        print("new_size={:.1f} MiB".format(df.memory_usage(deep=True).sum() / 1048576))
    return df


# Ukol 2: následky nehod v jednotlivých regionech
def plot_conseq(df: pd.DataFrame, fig_location: str = None,
                show_figure: bool = False):

    # set figure grid
    fig, ax = plt.subplots(4, sharex='all')
    fig.set_figheight(8)
    fig.set_figwidth(13)

    # Starting with last in order to get ordered regions list for axes
    f3 = df.loc[slice(None), ['region']]
    f3 = f3['region'].value_counts(ascending=False)
    f3.plot(ax=ax[3], kind='bar')
    ax[3].set_title('Celkom nehôd')
    ordered_regs = list(f3.index.values)

    # Deaths
    f0 = df.loc[slice(None), ['region', 'p13a']]
    f0 = f0.groupby(['region']).agg('sum')
    ax[0] = f0.loc[ordered_regs].plot(ax=ax[0], kind="bar", legend=False)
    ax[0].set_title('Úmrtia')

    # Injuries serious
    f1 = df.loc[slice(None), ['region', 'p13b']]
    f1 = f1.groupby(['region']).agg('sum')
    ax[1] = f1.loc[ordered_regs].plot(ax=ax[1], kind="bar", legend=False)
    ax[1].set_title('Ťažké zranenie')

    # Injuries light
    f2 = df.loc[slice(None), ['region', 'p13c']]
    f2 = f2.groupby(['region']).agg('sum')
    ax[2] = f2.loc[ordered_regs].plot(ax=ax[2], kind="bar", legend=False)
    ax[2].set_title('Ľahké zranenie')

    # Customize plots
    for i in range(4):
        ax[i].set_axisbelow(True)
        ax[i].yaxis.grid(color='gray', linestyle='dashed')
        ax[i].set_ylabel("Počet")
        ax[i].set_facecolor('#d4ebf2')

    plt.xticks(rotation='horizontal')
    plt.tight_layout()
    if show_figure:
        plt.show()
    plt.savefig(fig_location)


# Ukol 3: příčina nehody a škoda
def plot_damage(df: pd.DataFrame, fig_location: str = None,
                show_figure: bool = False):
    # 4 chosen regions
    kraje = ['JHM', 'HKK', 'PLK', 'PHA']
    df = df.loc[slice(None), ['region', 'p53', 'p12']]
    df = df.loc[df['region'].isin(kraje)]

    # Categorization
    df['p53'] = pd.cut(x=df['p53'], bins=[-1, 500, 2000, 5000, 10000, np.math.inf], labels=['< 50', '50 - 200', '200 - 500', '500 - 1000', '>1000'])
    df['p12'] = pd.cut(x=df['p12'], bins=[0, 100, 300, 400, 500, 600, 700], labels=['nezaviněná řidičem', 'nepřiměřená rychlost jízdy', 'nesprávné předjíždění', 'nedání přednosti v jízdě', 'nesprávný způsob jízdy', 'technická závada vozidla'])

    df = df.groupby(by=['region','p53', 'p12']).size().reset_index(name='Size')

    # set figure grid
    f = plt.figure(figsize=(14, 8))
    gs = f.add_gridspec(2, 2)

    # plot 4 regions into 2x2 grid
    with sns.axes_style("darkgrid"):
        ax = f.add_subplot(gs[0, 0])
        sns.barplot(data=df.loc[df['region'] == kraje[0]], x='p53', y='Size', hue='p12', ax=ax)
        ax.set(yscale='log', title=kraje[0], xlabel='', ylabel='počet')

        ax = f.add_subplot(gs[0, 1])
        sns.barplot(data=df.loc[df['region'] == kraje[1]], x='p53', y='Size', hue='p12', ax=ax)
        ax.set(yscale='log', title=kraje[1], xlabel='', ylabel='')

        ax = f.add_subplot(gs[1, 0])
        sns.barplot(data=df.loc[df['region'] == kraje[2]], x='p53', y='Size', hue='p12', ax=ax)
        ax.set(yscale='log', title=kraje[2], xlabel='škoda(v tisíc Kč)', ylabel='počet')

        ax = f.add_subplot(gs[1, 1])
        sns.barplot(data=df.loc[df['region'] == kraje[3]], x='p53', y='Size', hue='p12', ax=ax)
        ax.set(yscale='log',title=kraje[3], xlabel='škoda(v tisíc Kč)', ylabel='')

    # Customizing positions and setting legend
    f.subplots_adjust(right=0.8)
    lines, labels = f.axes[-1].get_legend_handles_labels()
    f.legend(lines, labels, loc='center right', bbox_to_anchor=(1, 0.5))  # bbox_to_anchor=(1.2,1)
    for ax in f.get_axes():
        ax.get_legend().remove()

    if show_figure:
        plt.show()
    f.savefig(fig_location)


# Ukol 4: povrch vozovky
def plot_surface(df: pd.DataFrame, fig_location: str = None,
                 show_figure: bool = False):
    # 4 chosen regions
    kraje = ['JHM', 'HKK', 'PLK', 'PHA']
    df = df.loc[slice(None), ['region', 'p16', 'date']] # desired information
    df = df.loc[df['region'].isin(kraje)].reset_index(drop=True)

    # categorization
    df['p16'] = pd.cut(x=df['p16'], bins=[-1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
                       labels=['jiný stav', 'suchý neznečištěný', 'suchý znečištěný',
                               'mokrý', 'bláto', 'náledí, ujetý sníh  - posypané', 'náledí, ujetý sníh  - neposypané',
                               'rozlitý olej, nafta apod.', 'souvislý sníh', 'náhlá změna stavu'])

    # change date to YYYY-MM  in string format
    df['date'] = df['date'].dt.strftime('%Y-%m')

    # grouping by params while counting accididents in same day
    df = df.groupby(by=['region', 'p16', 'date']).size().reset_index(name='Size')

    # setting date back to date type for plotting
    df['date'] = pd.to_datetime(df['date'])

    pivot = df.pivot_table(index=['region','p16','date'], values=['Size'], aggfunc='sum')

    # set figure grid
    f = plt.figure(figsize=(15, 8))
    gs = f.add_gridspec(2, 2)

    # plot 4 regions into 2x2 grid
    with sns.axes_style("darkgrid"):
        ax = f.add_subplot(gs[0, 0])
        sns.lineplot(data=pivot.loc[kraje[0]], x='date', y='Size', hue='p16', ax=ax)
        ax.set(title=kraje[0], xlabel='', ylabel='Počet nehod', xticklabels=[])

        ax = f.add_subplot(gs[0, 1], sharey=ax)
        sns.lineplot(data=pivot.loc[kraje[1]], x='date', y='Size', hue='p16', ax=ax)
        ax.set(title=kraje[1], xlabel='', ylabel='', xticklabels=[])

        ax = f.add_subplot(gs[1, 0], sharey=ax)
        sns.lineplot(data=pivot.loc[kraje[2]], x='date', y='Size', hue='p16', ax=ax)
        ax.set(title=kraje[2], xlabel='Datum vzniku nehody', ylabel='Počet nehod')

        ax = f.add_subplot(gs[1, 1], sharey=ax)
        sns.lineplot(data=pivot.loc[kraje[3]], x='date', y='Size', hue='p16', ax=ax)
        ax.set(title=kraje[3], xlabel='Datum vzniku nehody', ylabel='')

    # customize positions and set legend
    f.subplots_adjust(right=0.78)
    lines, labels = f.axes[-1].get_legend_handles_labels()
    f.legend(lines, labels, loc='center right', bbox_to_anchor=(0.98, 0.5))
    for ax in f.get_axes():
        ax.get_legend().remove()

    if show_figure:
        plt.show()
    f.savefig(fig_location)


if __name__ == "__main__":
    pass
    # zde je ukazka pouziti, tuto cast muzete modifikovat podle libosti
    # skript nebude pri testovani pousten primo, ale budou volany konkreni ¨
    # funkce.
    df = get_dataframe("accidents.pkl.gz")
    plot_conseq(df, fig_location="01_nasledky.png", show_figure=True)
    plot_damage(df, "02_priciny.png", True)
    plot_surface(df, "03_stav.png", True)
