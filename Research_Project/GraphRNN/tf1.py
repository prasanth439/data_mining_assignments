import torch
import torchvision as tv
import torch.nn as nn
from torch.autograd import Variable
from random import shuffle

import networkx as nx
import pickle as pkl
import logging

import random
import shutil
import os
import time
import numpy as np
import networkx as nx
import pickle

def convert_to_networkGraphs(data_file=None):
    if(data_file==None):
      print("No given datafile")
      exit()
		
    total_node = 0
    total_edge = 0
    total_graph = 0

    node_label_map = {}
    data_node_label = []
    nodelabelcount = 1
    graphs = []
    data_graph_labels = []
    nodelist_graph = []
    edgelist_graph = []
    num_node = 0
    num_edge = 0
    flag = 1
    f = open(data_file, "r")
    while True:
      line = f.readline()
      if line == '':
        break
      G = nx.Graph() 
      # print(line)
      # continueblob:https://colab.research.google.com/6cfbe4ac-6683-458e-9694-06d2a940dce2
      id_line = "#" in line
      if (id_line):
        data_graph_labels.insert(total_graph, 1)
        total_graph = total_graph + 1
        nodelist_graph = []
        edgelist_graph = []
        # reset nodelist_graph and edgelist_graph
        if (flag == 1):
          graph = {}
        else:
          flag = 0

        num_node = 0
        num_edge = 0
        # Reset all the variables

        num_node = int(f.readline(), 10)
        for x in range(num_node):
          node_iter = f.readline()
          node_flag = node_label_map.get(node_iter)
          if node_flag is None:
            node_label_map[node_iter]  = nodelabelcount
            nodelabelcount = nodelabelcount + 1
          # adding vertex to node_list
          # nodelist_graph.insert(x, node_label_map.get(node_iter))
          nodelist_graph.append(node_label_map.get(node_iter))

        num_edge = int(f.readline())

        for x in range (num_edge):
          edge_iter = f.readline().split()
          # edge = [int(edge_iter[0]) + 1 , int(edge_iter[1]) + 1 ]
          edge = [int(edge_iter[0]) , int(edge_iter[1]) ]
          edgelist_graph.insert(x, edge)

        edgelist_graph_tuple = list(map(tuple,edgelist_graph))
        G.add_edges_from(edgelist_graph_tuple)
        for i in range(len(nodelist_graph)):
          G.add_node(i, label = nodelist_graph[i])
        G.remove_nodes_from(list(nx.isolates(G)))
        total_edge = total_edge + num_edge
        total_node = total_node + num_node
        graphs.append(G)
    return graphs


savedatafile = ""
if savedatafile!="":
  datafile = open(savedatafile,"rb")
  graphs =pickle.load(datafile)
else:
  datafile = "input.txt"
  graphs = convert_to_networkGraphs(datafile)



def bfs_seq(G, start_id):
    dictionary = dict(nx.bfs_successors(G, start_id))
    start = [start_id]
    output = [start_id]
    while len(start) > 0:
        next = []
        while len(start) > 0:
            current = start.pop(0)
            neighbor = dictionary.get(current)
            if neighbor is not None:
                #### a wrong example, should not permute here!
                # shuffle(neighbor)
                next = next + neighbor
        output = output + next
        start = next
    return output



def encode_adj(adj, max_prev_node=10, is_full = False):
    if is_full:
        max_prev_node = adj.shape[0]-1

    # pick up lower tri
    adj = np.tril(adj, k=-1)
    n = adj.shape[0]
    adj = adj[1:n, 0:n-1]

    # use max_prev_node to truncate
    # note: now adj is a (n-1)*(n-1) matrix
    adj_output = np.zeros((adj.shape[0], max_prev_node))
    for i in range(adj.shape[0]):
        input_start = max(0, i - max_prev_node + 1)
        input_end = i + 1
        output_start = max_prev_node + input_start - input_end
        output_end = max_prev_node
        adj_output[i, output_start:output_end] = adj[i, input_start:input_end]
        adj_output[i,:] = adj_output[i,:][::-1] # reverse order

    return adj_output

class Graph_sequence_sampler_pytorch(torch.utils.data.Dataset):
    def __init__(self, G_list, max_num_node=None, max_prev_node=None, iteration=20000):
        self.adj_all = []
        self.len_all = []
        self.labels_all = []
        for G in G_list:
            self.adj_all.append(np.asarray(nx.to_numpy_matrix(G)))
            self.len_all.append(G.number_of_nodes())
            self.labels_all.append(list((nx.get_node_attributes(G,'label')).values()))
        # print(self.labels_all)
        if max_num_node is None:
            self.n = max(self.len_all)
        else:
            self.n = max_num_node
        if max_prev_node is None:
            print('calculating max previous node, total iteration: {}'.format(iteration))
            self.max_prev_node = max(self.calc_max_prev_node(iter=iteration))
            print('max previous node: {}'.format(self.max_prev_node))
        else:
            self.max_prev_node = max_prev_node
    def __len__(self):
        return len(self.adj_all)
    def __getitem__(self, idx):
        adj_copy = self.adj_all[idx].copy()
        label_copy = self.labels_all[idx].copy()
        label_batch = np.zeros(self.n,dtype=int)
        x_batch = np.zeros((self.n, self.max_prev_node))  # here zeros are padded for small graph
        x_batch[0,:] = 1 # the first input token is all ones
        y_batch = np.zeros((self.n, self.max_prev_node))  # here zeros are padded for small graph
        # generate input x, y pairs
        len_batch = adj_copy.shape[0]
        x_idx = np.random.permutation(adj_copy.shape[0])
        adj_copy = adj_copy[np.ix_(x_idx, x_idx)]
        adj_copy_matrix = np.asmatrix(adj_copy)
        label_copy = np.take(label_copy, x_idx)

        G = nx.from_numpy_matrix(adj_copy_matrix)
        # then do bfs in the permuted G
        start_idx = np.random.randint(adj_copy.shape[0])
        x_idx = np.array(bfs_seq(G, start_idx))
        adj_copy = adj_copy[np.ix_(x_idx, x_idx)]
        label_copy = np.take(label_copy, x_idx)

        adj_encoded = encode_adj(adj_copy.copy(), max_prev_node=self.max_prev_node)
        # get x and y and adj
        # for small graph the rest are zero padded
        y_batch[0:adj_encoded.shape[0], :] = adj_encoded
        x_batch[1:adj_encoded.shape[0] + 1, :] = adj_encoded
        label_batch[0:label_copy.shape[0]] = label_copy
        return {'x':x_batch,'y':y_batch, 'len':len_batch, 'lab':label_batch}
    def calc_max_prev_node(self, iter=20000,topk=10):
        max_prev_node = []
        for i in range(iter):
            if i % (iter / 5) == 0:
                print('iter {} times'.format(i))
            adj_idx = np.random.randint(len(self.adj_all))
            adj_copy = self.adj_all[adj_idx].copy()
            # print('Graph size', adj_copy.shape[0])
            x_idx = np.random.permutation(adj_copy.shape[0])
            adj_copy = adj_copy[np.ix_(x_idx, x_idx)]
            adj_copy_matrix = np.asmatrix(adj_copy)
            G = nx.from_numpy_matrix(adj_copy_matrix)
            # then do bfs in the permuted G
            start_idx = np.random.randint(adj_copy.shape[0])
            x_idx = np.array(bfs_seq(G, start_idx))
            adj_copy = adj_copy[np.ix_(x_idx, x_idx)]
            # encode adj
            adj_encoded = encode_adj_flexible(adj_copy.copy())
            max_encoded_len = max([len(adj_encoded[i]) for i in range(len(adj_encoded))])
            max_prev_node.append(max_encoded_len)
        max_prev_node = sorted(max_prev_node)[-1*topk:]
        return max_prev_node

dataset = Graph_sequence_sampler_pytorch(graphs,max_num_node=5,max_prev_node=5)
sample_strategy = torch.utils.data.sampler.WeightedRandomSampler([1.0 / len(dataset) for i in range(len(dataset))],
                                                                    num_samples=2*2, replacement=True)
dataset_loader = torch.utils.data.DataLoader(dataset, batch_size=2, num_workers=2,
                                            sampler=sample_strategy)

for batch_idx, data in enumerate(dataset_loader):
  print(batch_idx)
  print(data['lab'])