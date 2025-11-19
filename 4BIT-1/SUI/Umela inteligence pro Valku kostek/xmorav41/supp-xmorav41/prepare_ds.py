import os

import numpy as np

from dicewars.shared import DATASET
from dicewars.shared import TRAIN_FILE
from dicewars.shared import VALIDATE_FILE
from dicewars.shared import TEST_FILE

data = np.genfromtxt(DATASET, delimiter=",")

DATA_COUNT = data.shape[0]


train_data, test_data = np.split(data, [int(0.7 * len(data))])


np.random.shuffle(train_data)

with open(TRAIN_FILE,'w') as csvfile:        
    np.savetxt(csvfile, train_data,delimiter=',', fmt='%f', comments='')
with open(TEST_FILE,'w') as csvfile:        
    np.savetxt(csvfile, test_data,delimiter=',', fmt='%f', comments='')

