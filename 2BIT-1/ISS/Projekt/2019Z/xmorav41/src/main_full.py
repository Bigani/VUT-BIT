#############
#  ISS PROJ 2019 /20
#  Author: Tomáš Moravčík
#  Login: xmorav41
#############

import numpy as np
import matplotlib.pyplot as plt
import soundfile as sf
from scipy.signal import spectrogram
from scipy.stats.mstats_basic import pearsonr
from pydub import AudioSegment


# funckia na výpočet aritmetických priemerov prvkov
def feature(sgr0):
    features = np.empty((len(sgr0[1]), 16))
    i = 0
    result = 0
    while i < len(sgr0[1]):
        k = 0
        j = 0
        while j < 256:
            result = result + sgr0[j][i]
            if (j + 1) % 16 == 0:
                result = result / 16
                features[i][k] = result
                k += 1
                result = 0
            j += 1
        i += 1
    return features


### main ###
name_list = ['sa1.wav', 'sa2.wav', 'si711.wav', 'si1341.wav', 'si1841.wav', 'sx81.wav', 'sx171.wav', 'sx261.wav',
             'sx351.wav', 'sx441.wav']
name_list_save = ['sa1.png', 'sa2.png', 'si711.png', 'si1341.png', 'si1841.png', 'sx81.png', 'sx171.png', 'sx261.png',
                  'sx351.png', 'sx441.png']

sentence = 0  # číslo nahrávky 0 - 9

while sentence < 10:           # loop pre prejdenie všetkými nahrávkami, na spodu kod na ich uloženie ako png
    s, fs = sf.read(name_list[sentence])
    s1, fs1 = sf.read('q1.wav')     # however
    s2, fs2 = sf.read('q2.wav')     # yellow


    f, t, sgr = spectrogram(s, fs, nperseg=400, noverlap=240, nfft=511)       #  400 240 511
    f1, t1, sgr1 = spectrogram(s1, fs1, nperseg=400, noverlap=240, nfft=511)  #  400 240 511
    f2, t2, sgr2 = spectrogram(s2, fs2, nperseg=400, noverlap=240, nfft=511)  #  400 240 511

    sgr = 10 * np.log10(sgr + 1e-20)
    sgr1 = 10 * np.log10(sgr1 + 1e-20)
    sgr2 = 10 * np.log10(sgr2 + 1e-20)

    features_s = feature(sgr)       # veta
    features_w = feature(sgr1)      # query1
    features_w2 = feature(sgr2)     # query2

    counter = 0                                     # počítadlo pre výpočet koeficientu
    n_of_rep = len(features_s) - len(features_w)    # počet opakovaní, rovná sa dlžke vety mínus dĺžke slova aby slovo "nepretieklo" za vetu
    list1 = np.empty((len(sgr1[1]), 16))            # segment z vety
    list2 = []                                      # array s koeficientmi podobnosti
    counter2 = 0                                    # počítadlo pre naberanie vektorov segmentu vety
    hit = 1

    # prve query
    while counter < n_of_rep:
        while counter2 < len(features_w):                           # segment o dĺžke slova
            list1[counter2] = features_s[counter + counter2]        # vloženie vektoru do arraye
            counter2 += 1                                           # navýšenie počítadla

        kf = pearsonr(list1, features_w)                            # výpočet pearsonovho korelačného koeficientu
        if kf[0] > 0.93:                                                                                     # empiricky zvolená hodnota
            print('QUERY 1 zhoda :', kf[0], ' na prvku: ', counter, ' veta ',name_list[sentence])            # sprava na terminal
            t1 = counter * 10                                                                                # odkial vytvori snippet nahravky
            t2 = (counter + counter2) * 10                                                                   # pokial vytvori snippet nahravky
            newAudio = AudioSegment.from_wav(name_list[sentence])                                            # uloží audio vety do premennej
            newAudio = newAudio[t1:t2]                                                                       # osekne ho na daný snippet
            hit_name = str(hit) + '_q1_' + name_list[sentence]                                               # vygeneruje názov hitu
            newAudio.export(hit_name, format="wav")                                                          # exportne hit
            hit += 1
        list2.append(kf[0])                                         # koeficient sa vloží do arraye
        counter += 1                                                # navýšenie počítadla
        counter2 = 0                                                # vynulovanie počítadla

    counter_1 = 0
    counter_2 = 0
    n_of_rep = len(features_s) - len(features_w2)
    list_1 = np.empty((len(sgr2[1]), 16))
    list_2 = []
    hit = 1
    # druhe query
    while counter_1 < n_of_rep:
        while counter_2 < len(features_w2):
            list_1[counter_2] = features_s[counter_1 + counter_2]
            counter_2 += 1


        kf = pearsonr(list_1, features_w2)
        if kf[0] > 0.92:
            print('QUERY 2 zhoda :',kf[0], ' na prvku: ', counter_1, ' veta ',name_list[sentence])
            t1 = counter_1 * 10
            t2 = (counter_1 + counter_2) * 10
            newAudio = AudioSegment.from_wav(name_list[sentence])
            newAudio = newAudio[t1:t2]
            hit_name = str(hit) + '_q2_' + name_list[sentence]
            hit += 1
            newAudio.export(hit_name, format="wav")
        list_2.append(kf[0])
        counter_2 = 0
        counter_1 += 1


    ####### Vykreslenie

    features_s = features_s.transpose()
    # features_s = features_s.transpose()      # tu nie je potreba
    # features_w = features_w.transpose()      # tu nie je potreba

    fig, ax = plt.subplots(3)

    title = " 'yellow' and 'however' vs " + name_list[sentence]
    ax[0].set_title(title)

    ax[1].pcolormesh(t, range(0, 16), features_s)
    ax[1].set_ylabel('features')
    ax[1].invert_yaxis()
    ax[1].set_xlabel('t')

    s = s[:250000]
    t = np.arange(s.size) / fs

    ax[0].plot(t, s)
    ax[0].set_xlabel('t')
    ax[0].set_ylabel('signal')
    ax[0].set_xlim([0, len(t) / 16000])

    ax[2].plot(np.arange(len(list2)) / 100, list2, label='However')
    ax[2].plot(np.arange(len(list_2)) / 100, list_2, label='Yellow')
    ax[2].set_xlim([0, len(t) / 16000])
    ax[2].legend()
    ax[2].set_ylabel('scores')
    ax[2].set_xlabel('t')

    plt.tight_layout()

    fig.set_size_inches(10.5, 5.5)

    plt.show()

    #plt.savefig(name_list_save[sentence], bbox_inches='tight')     # png file save
    sentence += 1
