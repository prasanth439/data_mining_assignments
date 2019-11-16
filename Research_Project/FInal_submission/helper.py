import networkx as nx
import numpy as np

def getAdjMatNormal(graph):
    return np.asarray(nx.to_numpy_matrix(graph))

def getAdjMatNp(graph):
    return nx.to_numpy_matrix(graph)
'''
params : network graphs
return : max nuber of Nodes in graphs list
'''
def getMaxNodes(graph_list):
    return max([len(g.nodes()) for g in graph_list])

if '__name__'=='__main__':
    pass