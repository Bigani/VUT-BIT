import os
import io
import zipfile
import csv
import re
import datetime
import pickle
import gzip
import shutil

from bs4 import BeautifulSoup
import numpy as np
import requests


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


# Trieda pre prácu s datami, zahrňuje ich stiahnutie a parsovanie
class DataDownloader:

    def __init__(self, url="https://ehw.fit.vutbr.cz/izv/", folder="data", cache_filename="data_{}.pkl.gz"):
        self.THIS_FOLDER = os.path.dirname(os.path.abspath(__file__))
        self.url = url
        self.DATA_FOLDER = self.THIS_FOLDER + "/" + folder
        self.cache_filename = cache_filename
        self.makedir(folder)
        self.CACHE_FOLDER = self.DATA_FOLDER + "/cache/"
        self.makedir(self.CACHE_FOLDER)
        self.parsed_regions = []        # 3znakove skratky
        # Data columns names
        # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
        self.col_names = "p1 p36 p37 p2a weekday(p2a) p2b p6 p7 p8 p9 p10 p11 p12 p13a p13b p13c p14 p15 p16  \
                         p17 p18 p19 p20 p21 p22 p23 p24 p27 p28 p34 p35 p39 p44 p45a p47 p48a p49 p50a p50b  \
                         p51 p52 p53 p55a p57 p58 a b d e f g h i j k l n o p q r s t p5a region"
        self.col_names = self.col_names.split()
        self.valid_regions = "PHA STC JHC PLK KVK ULK LBK HKK PAK OLK MSK JHM ZLK VYS"
        self.valid_regions = self.valid_regions.split()
        self.valid_regions_tuples = (PHA, STC, JHC, PLK, KVK, ULK, LBK, HKK, PAK, OLK, MSK, JHM, ZLK, VYS)
        self.data = (self.col_names, [])

    # stiahne zip súbory zo stránky podľa rokov a rozbalí ich do priečinku
    def download_data(self):
        html = requests.get(url="https://ehw.fit.vutbr.cz/izv/",
                            headers={'User-Agent': 'Mozilla/5.0 (X11; Linux x86_64 ...'})
        pattern = "[^0-9].[0-9][0-9][0-9][0-9].zip"
        pattern2 = "09-2020.zip"        # natvrdo lebo neupdatnute konzistentne
        soup = BeautifulSoup(html.text, features="html.parser")

        for anchor in soup.findAll('a', href=True):
            links = self.url + anchor['href']
            if re.search(pattern, links) or re.search(pattern2, links):
                r = requests.get(links)
                if not r.ok:
                    continue
                datapath = self.DATA_FOLDER + "/" + links[34:-4]
                z = zipfile.ZipFile(io.BytesIO(r.content))
                self.makedir(datapath)
                z.extractall(datapath)

    # stiahnuté súborý načíta a sparsuje
    def parse_region_data(self, region):
        if not self.check_download():
            self.download_data()
        region_data_path = []                   # list s cestami k suborom
        cache_tuple = (self.col_names, [])      # potrebujem cisty tuple pre cache

        for dirpath, dirnames, filenames in os.walk("./data"):      # hladaj v adresaroch subory
            for filename in [f for f in filenames if f.endswith(region[0] + ".csv")]:
                region_data_path.append(os.path.join(dirpath, filename))
                break

        # pre kazdy subor parsuj
        for path in region_data_path:
            with open(path, encoding="ISO-8859-2") as csvfile:
                c_reader = csv.reader(csvfile, delimiter=';')
                for row in c_reader:
                    count = 0       # poloha v riadku na stlpci
                    # konvertuj retazce na prislusnejsie typy
                    for i in row:
                        if count <= 2:
                            try:
                                row[count] = int(i)
                            except ValueError:
                                row[count] = None
                        elif count == 3:
                            try:
                                date_obj = datetime.datetime.strptime(i, "%Y-%m-%d")
                                row[count] = date_obj.date()
                            except ValueError:
                                row[count] = None
                        elif count == 4:
                            try:
                                row[count] = int(i)
                            except ValueError:
                                row[count] = None
                        elif count == 5:
                            try:
                                time_obj = datetime.datetime.strptime(i, "%H%M")
                                row[count] = time_obj.time()
                            except ValueError:
                                row[count] = None
                        elif 5 < count <= 34:
                            try:
                                row[count] = int(i)
                            except ValueError:
                                row[count] = None
                        # 35 skip string
                        elif 35 < count <= 44:
                            try:
                                row[count] = int(i)
                            except ValueError:
                                row[count] = None
                        elif 44 < count <= 50:
                            try:
                                row[count] = float(i.replace(",", "."))
                            except ValueError:
                                row[count] = None
                        elif count == 63:
                            try:
                                row[count] = int(i)
                            except ValueError:
                                row[count] = None
                        count += 1
                    row.append(region[1])  # add region
                    arr = np.array(row)
                    self.data[1].append(arr)
                    cache_tuple[1].append(arr)
        self.parsed_regions.append(region[0])
        cache_filename = self.cache_filename.replace("{}", region[1])
        pickle.dump(cache_tuple, gzip.open(self.CACHE_FOLDER + cache_filename, 'wb'))

    # Získa data pre chcené regióny
    def get_list(self, regions=None):
        # všetky kraje
        if regions is None:
            regions = self.valid_regions_tuples
        # špecifické kraje
        for region in regions:
            if region[1] not in self.valid_regions:        # ignoruj invalidne regiony
                continue

            # 1) check attribute
            # # # # #
            if region[0] in self.parsed_regions:
                continue

            # 2) check cache
            # # # # #
            if os.path.exists(self.CACHE_FOLDER + "data_" + region[1] + ".pkl.gz"):
                f = pickle.load(gzip.open(self.CACHE_FOLDER + "data_" + region[1] + ".pkl.gz", 'rb'))
                self.data[1].extend(f[1])
                self.parsed_regions.append(region[0])
                continue

            # 3) parse
            # # # # #
            self.parse_region_data(region)

        # self.synopsis(regions)
        return self.data

    # vymaže súbor
    def rmvdir(self, folder="data"):
        os.rmdir(folder)

    # vytvorí súbor
    def makedir(self, folder):
        try:
            if not os.path.isdir(folder):
                oldmask = os.umask(000)
                os.makedirs(folder, 0o777)
                os.umask(oldmask)
            os.chmod(folder, 0o777)
        except FileNotFoundError:
            print("FileNotFoundError error")
            exit(0)

    # vráti do prvotného stavu
    def clear(self):
        if os.path.exists(self.DATA_FOLDER):
            shutil.rmtree(self.DATA_FOLDER)

    # meta data parsovania
    def synopsis(self, regions):
        print("Parse Results\n<><><><><><><><>")
        print("Data columns:")
        print(self.col_names)
        print("<><><><><><><><>")
        print("Data log count: ", len(self.data[1]))
        print("<><><><><><><><>")
        print("Regions parsed: ")
        for each in regions:
            print(each[1], end=' ')
        print("\n\nEND OF LOG")

    # kontrola prítomnosti súborov
    def check_download(self):
        folders_name = ["/datagis-09-2020/", "/datagis-rok-2017/", "/datagis-rok-2018/", "/datagis-rok-2019/", "/datagis2016/"]
        for f in folders_name:
            if os.path.exists(self.DATA_FOLDER + f) and os.path.isdir(self.DATA_FOLDER + f):
                if not os.listdir(self.DATA_FOLDER + f):
                    return False
            else:
                return False
        return True

# testing
"""
POG = ("EQE","UWU")
test_obj = DataDownloader()
# test_obj.clear()
#test_obj.download_data()
#exit(0)
# test_obj.parse_region_data(PHA)
# test_tuple = test_obj.get_list([POG])
test_tuple = test_obj.get_list([PHA])
print("UwU")
exit(0)
"""


def main():
    reg_list = [PHA, ULK, KVK]

    test = DataDownloader()
    test_tuple = test.get_list(reg_list)
    test.synopsis(reg_list)


if __name__ == "__main__":
    main()


