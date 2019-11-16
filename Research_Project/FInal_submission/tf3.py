from __future__ import unicode_literals, print_function, division
from io import open
import unicodedata
import string
import re
import random

import torch
import torch.nn as nn
from torch.autograd import Variable
from torch import optim
import torch.nn.functional as F
import torch.nn.init as init
from torch.nn.utils.rnn import pad_packed_sequence, pack_padded_sequence

from collections import OrderedDict
import math
import numpy as np
import time


class MyRNN(nn.Module):
    def __init__(self, input_size, hidden_size, num_layers):
        super(MyRNN, self).__init__()
        self.rnn = nn.GRU(input_size=input_size, hidden_size=hidden_size, num_layers=num_layers, batch_first=True).cuda()
        self.hidden = None
        self.num_layers = num_layers 
        self.hidden_size = hidden_size
        for name, param in self.rnn.named_parameters():
            if 'bias' in name:
                nn.init.constant_(param, 0.25)
            elif 'weight' in name:
                nn.init.xavier_uniform_(param,gain=nn.init.calculate_gain('sigmoid'))

    def init_hidden(self, batch_size):
        return Variable(torch.zeros(self.num_layers, batch_size, self.hidden_size)).cuda()

    def forward(self, input_raw):
        inputp = input_raw
        output_raw, self.hidden = self.rnn(inputp, self.hidden)
        return output_raw



class Net(nn.Module):
    def __init__(self):
        super(Net, self).__init__()
        self.fc1 = nn.Linear(16 * 5 * 5, 120)
        self.fc2 = nn.Linear(120, 84)
        self.fc3 = nn.Linear(84, 10)

    def forward(self, x):
        x = x.view(-1, 16 * 5 * 5)
        x = F.relu(self.fc1(x))
        x = F.relu(self.fc2(x))
        x = self.fc3(x)
        return x
# dataset
# batch_size = 4
# time_steps = 5
# features = 3
# layers = 4
# hidden_size = 6

# bp = torch.zeros(batch_size,time_steps,features)

# inp = Variable(bp).cuda()

# myrnn = MyRNN(3,6,4)
# optimizer_rnn = optim.Adam(list(myrnn.parameters()), lr=0.03)
# myrnn.train()
# myrnn.zero_grad()
# myrnn.hidden = myrnn.init_hidden(batch_size)
# outp = myrnn(inp)
# optimizer_rnn.step()
# print(outp.size())

los = [i for i in range(60)]
tos = torch.Tensor(los).cuda()
tos = tos.view(5,4,3)
tos = tos.view(-1,3)
print(tos)