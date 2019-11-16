import numpy as np
import networkx as nx
import args
from helper import *
import random
import pickle
import pprint
# from params import *

'''
graph : network graph
return : list of nodes traversed in bfs
'''

'''
datafile : string of file name
returns : graph list
'''
def convert_to_networkGraphs(data_file=None):
    if(data_file==None):
      print("No given datafile")
      exit()
		
    total_node = 0
    total_edge = 0
    total_graph = 0

    node_label_map = {}
    edge_label_map = {}
    data_node_label = []
    nodelabelcount = 1
    edgelabelcount = 1
    graphs = []
    data_graph_labels = []
    nodelist_graph = []
    edgelist_graph = []
    node_reverse_map = []
    edge_reverse_map = []
    num_node = 0
    num_edge = 0
    flag = 1
    f = open(data_file, "r")
    while True:
      line = f.readline()
      if line == '':
        break
      G = nx.Graph() 
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
        # print('num nodes ',num_node)
        for x in range(num_node):
          node_iter = f.readline().split()
          node_flag = node_label_map.get(node_iter[0])
          # print(node_iter[0])
          if node_flag is None:
            node_label_map[node_iter[0]]  = nodelabelcount
            node_reverse_map.append(node_iter[0])
            nodelabelcount = nodelabelcount + 1
          # adding vertex to node_list
          # nodelist_graph.insert(x, node_label_map.get(node_iter[0]))
          nodelist_graph.append(node_label_map.get(node_iter[0]))

        num_edge = int(f.readline())
        # print('num edges ',num_edge)
        for x in range (num_edge):
          edge_iter = f.readline().split()
          # edge = [int(edge_iter[0]) + 1 , int(edge_iter[1]) + 1 ]
          edge_flag = edge_label_map.get((edge_iter[2]))
          if edge_flag is None:
            edge_label_map[(edge_iter[2])] = edgelabelcount
            edge_reverse_map.append(edge_iter[2])
            edgelabelcount = edgelabelcount + 1
          edge = [int(edge_iter[0]) , int(edge_iter[1]) , edge_label_map[(edge_iter[2])] ]
          edgelist_graph.append(edge)

        edgelist_graph_tuple = list(map(tuple,edgelist_graph))
        # G.add_edges_from(edgelist_graph_tuple)
        for i in range(len(nodelist_graph)):
          G.add_node(i, label = nodelist_graph[i])
        for i in range(len(edgelist_graph)):
          G.add_edge(edgelist_graph[i][0],edgelist_graph[i][1],label=edgelist_graph[i][2])
        G.remove_nodes_from(list(nx.isolates(G)))
        total_edge = total_edge + num_edge
        total_node = total_node + num_node
        graphs.append(G)
        # exit()
    return graphs, nodelabelcount, edgelabelcount,node_label_map,edge_label_map,node_reverse_map,edge_reverse_map
'''
params : network graphs
returns : adjacency matrices from network graphs
'''
def getAdjMatList(graphList):
  adj_list = []
  for g in graphList:
    adj_list.append(getAdjMatNormal(g))
  return adj_list



'''
params : adjMatrices
returns : maxHistory
'''
def calculateHistory(adjMatrices):
  history = 0
  numMatrices = len(adjMatrices) #number of matrices
  for iter in range(HISTORY_ITERATIONS):
    mat_id = random.randint(0,numMatrices)
    adj_dup = adjMatrices[mat_id].copy()
    node_indexes = [i for i in range(len(adj_dup))]
    per_node_indexes = np.random.permutation(node_indexes) # permuted node indexex
    adj_dup = adj_dup[np.ix_(per_node_indexes,per_node_indexes)]
    node_index = random.randint(0,len(adj_dup))
    G = nx.from_np(adj_dup)
    bfs_seq = get_bfs_seq(G,node_index)
    adj_dup = adj_dup[np.ix_(bfs_seq,bfs_seq)]
    encoded_matrix = get_encode_variable(adj_dup.copy())
    local_history = max([len(row) for row in encoded_matrix])
    history = max(history,local_history)
  return history

def bfs_seq(G, start_id):
    '''
    get a bfs node sequence
    :param G:
    :param start_id:
    :return:
    '''
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

if __name__ == '__main__':
  data_file = 'dataset/500_graph.txt'
  saved = True

  if not saved:
    graphs,_,_,_,_ = convert_to_networkGraphs(data_file)
    pickle_file = 'graphs/yeast_save'
    fil_writer = open(pickle_file,'wb')
    pickle.dump(graphs,fil_writer)
    fil_writer.close()
  else:
    pickle_file = 'graphs/yeast_save'
    file_reader = open(pickle_file,'rb')
    graphs = pickle.load(file_reader)
    file_reader.close()
  print(np.asarray(nx.to_numpy_matrix(graphs[0])))
  hell = nx.attr_matrix(graphs[0],edge_attr='label')[0]
  hell = np.asmatrix(hell)
  G = nx.from_numpy_matrix((hell))
  ktr = np.random.randint(hell.shape[0])
  seq2 = bfs_seq(graphs[0],ktr)
  seq = bfs_seq(G,ktr)
  print(seq)
  print(seq2==seq)
  # print(nx.get_edge_attributes(graphs[0],'label'))
  # print(nx.get_node_attributes(graphs[0],'label'))
  