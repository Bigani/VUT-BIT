#############
#  ISS PROJ 2019 /20
#  Author: Tomáš Moravčík
#  Login: xmorav41
#############

import numpy as np
import matplotlib.pyplot as plt
import soundfile as sf
from scipy.signal import spectrogram

s, fs = sf.read('sx171.wav')    # with however

"""
odkud = 0  # začátek segmentu v sekundách
kolik = 0.025  # délka segmentu v sekundách

odkud_vzorky = int(odkud * fs)  # začátek segmentu ve vzorcích
pokud_vzorky = int((odkud + kolik) * fs)  # konec segmentu ve vzorcích

s_seg = s[odkud_vzorky:pokud_vzorky]
N = s_seg.size

s_seg_spec = np.fft.fft(s_seg)
G = 10 * np.log10(1 / N * np.abs(s_seg_spec) ** 2)  # transformace
"""

f, t, sgr = spectrogram(s, fs, nperseg=400, noverlap=240, nfft=511, scaling='density')  #  400 240 511

features = np.empty((len(sgr[1]), 16))  # np array do ktorej budem vkladat aritmeticke priemery

sgr = 10 * np.log10(sgr + 1e-20)
i = 0
result = 0
while i < len(sgr[1]):
    k = 0
    j = 0
    while j < 256:
        result = result + sgr[j][i]
        if (j + 1) % 16 == 0:
            result = result / 16
            features[i][k] = result
            k += 1
            result = 0
        j += 1
    i += 1

features = features.transpose()

plt.figure(figsize=(9, 3))

plt.pcolormesh(t, f[::16], features)
plt.gca().set_xlabel('Čas [s]')
plt.gca().set_ylabel('Frekvence [Hz]')
plt.gca().set_title('sx171')
# cbar = plt.colorbar()
# cbar.set_label('Spektralní hustota výkonu [dB]', rotation=270, labelpad=15)

plt.tight_layout()
plt.show()

