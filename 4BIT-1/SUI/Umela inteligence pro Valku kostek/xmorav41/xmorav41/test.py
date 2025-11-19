import logging
import random
import torch
import numpy 

print(torch.__version__)


model = torch.load('./model.pth', map_location=torch.device('cpu'))
game = [17, 6,  2,  7,  1,  6,  7,  0, 28, 11,  2, 12,  2, 10,  4,  6, 11,  2,  2,  3,  2,  1,  3,  1, 27, 12,  1, 12,  1, 11,  3,  6]
print(type(game))
game = [game]
print(type(game))
game = numpy.array(game)
print(type(game))
game = torch.from_numpy(game).float()
print(model(game))