#!/usr/bin/python3.8
# coding=utf-8
import pandas as pd
import geopandas
import matplotlib.pyplot as plt
import contextily as ctx
import sklearn.cluster
import numpy as np


def make_geo(df: pd.DataFrame) -> geopandas.GeoDataFrame:
    df = df.dropna(subset=['d', 'e'])
    return geopandas.GeoDataFrame(df,
                                  geometry=geopandas.points_from_xy(df["d"],
                                                                    df["e"]),
                                  crs="EPSG:5514")


def plot_geo(gdf: geopandas.GeoDataFrame, fig_location: str = None,
             show_figure: bool = False):
    kraj = ['HKK']
    gdf = gdf.loc[gdf['region'].isin(kraj)]
    gdf1 = gdf[gdf['p5a'] == 1]
    gdf2 = gdf[gdf['p5a'] == 2]

    fig, ax = plt.subplots(nrows=1, ncols=2, figsize=(10, 4))
    gdf1.plot(ax=ax[0], markersize=3,
              color="tab:red", label="Nehody v HKK kraji: v obci")
    ax[0].axis('off')
    ax[0].set_title('Nehody v HKK kraji: v obci')
    ctx.add_basemap(ax[0], crs=gdf.crs.to_string(),
                    source=ctx.providers.Stamen.TonerLite, alpha=.9)
    gdf2.plot(ax=ax[1], markersize=3, color="tab:green",
              label="Nehody v HKK kraji: mimo obec")
    ax[1].axis('off')
    ax[1].set_title('Nehody v HKK kraji: mimo obec')
    ctx.add_basemap(ax[1], crs=gdf.crs.to_string(),
                    source=ctx.providers.Stamen.TonerLite, alpha=.9)

    plt.tight_layout()
    plt.subplots_adjust(0.07, 0.04, 0.97, 0.96, 0.36, 0.20)

    plt.savefig(fig_location)
    if show_figure:
        plt.show()


def plot_cluster(gdf: geopandas.GeoDataFrame, fig_location: str = None,
                 show_figure: bool = False):
    kraj = ['ZLK']
    gdf = gdf.loc[gdf['region'].isin(kraj)]

    gdf_c = gdf.to_crs(epsg=5514)
    gdf_c = gdf_c.set_geometry(gdf_c.centroid).to_crs(epsg=3857)

    coords = np.dstack([gdf_c.geometry.x, gdf_c.geometry.y]).reshape(-1, 2)
    model = sklearn.cluster.MiniBatchKMeans(n_clusters=20).fit(coords)

    gdf4 = gdf_c.copy()
    gdf4['cluster'] = model.labels_

    gdf4 = gdf4.dissolve(by='cluster',
                         aggfunc={"p1": "count"}).rename(columns={"p1": "cnt"})

    gdf_coords = geopandas.GeoDataFrame(
        geometry=geopandas.points_from_xy(model.cluster_centers_[:, 0],
                                          model.cluster_centers_[:, 1]))

    gdf5 = gdf4.merge(gdf_coords, left_on='cluster',
                      right_index=True).set_geometry('geometry_y')

    plt.figure(figsize=(10, 8))
    ax = plt.gca()
    plt.axis('off')

    gdf = gdf.to_crs(epsg=3857)
    gdf.plot(ax=ax, markersize=2, color="tab:grey", alpha=0.5)
    gdf5.plot(ax=ax, markersize=gdf5["cnt"], column="cnt",
              legend=True, alpha=0.5)
    ctx.add_basemap(ax, crs='epsg:3857',
                    source=ctx.providers.Stamen.TonerLite, alpha=0.8)

    ax.set_title('Nehody v ZLK kraji')

    plt.savefig(fig_location)
    if show_figure:
        plt.show()


if __name__ == "__main__":
    # zde muzete delat libovolne modifikace
    gdf = make_geo(pd.read_pickle("accidents.pkl.gz"))
    plot_geo(gdf, "geo1.png", True)
    plot_cluster(gdf, "geo2.png", True)
