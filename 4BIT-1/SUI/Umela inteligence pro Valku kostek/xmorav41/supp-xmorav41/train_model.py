import torch
import numpy as np
import torch.nn as nn
import torch.nn.functional as F
import torch.optim as optim
from sklearn.metrics import  accuracy_score

from cnn import *
from dicewars.shared import TRAIN_FILE
from dicewars.shared import TEST_FILE


def round_tensor(t, decimal_places=3):
      return round(t.item(), decimal_places)


def multi_acc(y_pred, y_test):
    y_pred_softmax = torch.log_softmax(y_pred, dim=1)
    _, y_pred_tags = torch.max(y_pred_softmax, dim=1)
    correct_pred = (y_pred_tags == y_test).float()

    acc = correct_pred.sum() / len(correct_pred)

    acc = torch.round(acc * 100)

    return acc
         
    
train_data = np.genfromtxt(TRAIN_FILE, delimiter=",")
train_inputs, train_labels = np.split(train_data,[32], axis=1)

test_data = np.genfromtxt(TEST_FILE, delimiter=",")
test_inputs, test_labels = np.split(test_data,[32], axis=1)


test_labels = np.array([(y[0]*10 - 5) if y[0] != 0 else y[0] for y in test_labels.tolist() ])
train_labels = np.array([(y[0]*10 - 5) if y[0] != 0 else y[0] for y in train_labels.tolist() ])

X_train = torch.from_numpy(train_inputs).float()
y_train = torch.squeeze(torch.from_numpy(train_labels)).float()

net = Net(X_train.shape[1])



X_test = torch.from_numpy(test_inputs).float()
y_test = torch.squeeze(torch.from_numpy(test_labels)).float()

criterion = nn.NLLLoss()
optimizer = optim.Adam(net.parameters(), lr=0.001, weight_decay=0.001)

device = "cpu" #torch.device("cuda:0" if torch.cuda.is_available() else "cpu")
X_train = X_train.to(device)
y_train = y_train.to(device)
X_test = X_test.to(device)
y_test = y_test.to(device)
net = net.to(device)
criterion = criterion.to(device)


y_test = y_test.type(torch.LongTensor).to(device)
y_train = y_train.type(torch.LongTensor).to(device)
for epoch in range(500):
    y_pred = net(X_train)
    y_pred = torch.squeeze(y_pred).to(device)
    train_loss = criterion(y_pred.cpu(), y_train.cpu())
    if epoch % 100 == 0:
        
        
        temp = 0.0
        d_class = -1
        y_pred_list = []
        for cnt, j in enumerate(y_pred):
            for i in range(0,6):
                p_to_be_in_certain_part_of_winning_game = j[i].tolist()
                if (p_to_be_in_certain_part_of_winning_game > temp ) and (p_to_be_in_certain_part_of_winning_game >0.5):
                    temp = p_to_be_in_certain_part_of_winning_game
                    d_class = i
            y_pred_list.append(d_class)
            

        print('Training Accuracy: {:.2f}\n'.format(accuracy_score(y_train.cpu().detach().numpy(), y_pred_list)))
        y_test_pred = net(X_test).to(device)
        y_test_pred = torch.squeeze(y_test_pred).to(device)
        test_loss = criterion(y_test_pred, y_test)
        test_acc = multi_acc( y_test_pred,y_test)
        # print('Training Accuracy: {:.2f}\n'.format(accuracy_score(y_test.tensor().detach().numpy(), y_test_pred)))
    optimizer.zero_grad()
    train_loss.backward()
    optimizer.step()
    
MODEL_PATH = 'dicewars/ai/xmorav41/model.pth'
torch.save(net, MODEL_PATH)
