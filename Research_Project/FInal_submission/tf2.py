import numpy as np
import torch 
import torchvision
import networkx as nx
from torch.nn.utils.rnn import pad_packed_sequence, pack_padded_sequence
import pickle
from process_dataset import produce_graphs

file_name = 'input_format.txt'
g = open(file_name,'r')
