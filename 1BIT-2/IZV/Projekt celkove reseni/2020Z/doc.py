import pandas as pd
import numpy as np
from matplotlib import pyplot as plt
import seaborn as sns
from tabulate import tabulate

if __name__ == '__main__':
    df = pd.read_pickle('accidents.pkl.gz', compression='gzip')
    # druh pozemnej komunikacie / pocasie
    df = df.loc[slice(None), ['p37', 'p19']]

    df['p37'].replace('', np.nan, inplace=True)
    df.dropna(subset=['p37'], inplace=True)
    df['p37'] = df['p37'].astype(int)
    df['p37'] = pd.cut(x=df['p37'], bins=[-1, 100, 1000, np.math.inf],
                       labels=['1. trieda', '2. trieda', '3. trieda'])

    df['p19'] = pd.cut(x=df['p19'], bins=[0, 1, 2, 3, 4, 5, 6, 7],
                       labels=['deň; nezhoršené',
                               'deň; zhoršené (ráno/večer)',
                               'deň; zhoršené (hmla/sneh)',
                               'noc; nezhoršené osvetlené',
                               'noc; zhoršené (hmla/dážď)',
                               'noc; nezhoršené neosvetlené',
                               'noc; zhoršené neosvetlené'])

    bf = df.groupby('p37').count()
    lf = df.groupby('p19').count()

    print(tabulate(bf, headers=['Komunikacia', 'Pocet nehod'],
                   tablefmt='psql'))
    print('\n')
    lf = lf.rename(columns={"p19": "Viditelnost", "p37": "Pocet nehod", })
    print(tabulate(lf, headers=["Viditelnost", "Pocet nehod"],
                   tablefmt='psql'))

    df = df.groupby(by=['p37', 'p19']).size().reset_index(name='Size')

    f = plt.figure(figsize=(12, 6))
    gs = f.add_gridspec(1, 1)

    with sns.axes_style("darkgrid"):
        ax = f.add_subplot(gs[0, 0])
        sns.barplot(data=df, x='p19', y='Size', hue='p37', ax=ax,
                    palette='flare')
        ax.set(yscale='log',
               title='Nehodovosť podľa typu pozemnej komunikácie',
               xlabel='', ylabel='počet nehôd')
        plt.setp(ax.get_xticklabels(), rotation=30,
                 horizontalalignment='right')
        ax.legend().set_title('Komunikácia')

    plt.tight_layout()
    # plt.show()
    f.savefig('fig.png')
