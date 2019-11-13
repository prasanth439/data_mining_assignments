import networkx as nx
import numpy as np
import torch
import torch.nn as nn
import torch.nn.init as init
from torch.autograd import Variable
import torch.nn.functional as F
from torch import optim
from torch.optim.lr_scheduler import MultiStepLR
import logging
from time import gmtime, strftime
from random import shuffle
import pickle
# from tensorboard_logger import configure, log_value
import time as tm

# from utils import *
from model import *
from data import *
from args import Args
import networkx as nx
def get_label_graph(adj,node_labels,lab_adj):
    '''
    get a graph from zero-padded adj
    :param adj:
    :return:
    '''
    # remove all zeros rows and column
    adj = adj[~np.all(adj == 0, axis=1)]
    adj = adj[:, ~np.all(adj == 0, axis=0)]
    lab_adj = lab_adj[~np.all(lab_adj == 0, axis=1)]
    lab_adj = lab_adj[:, ~np.all(lab_adj == 0, axis=0)]
    adj = np.asmatrix(adj)
    G = nx.from_numpy_matrix(adj)
    label_dict = dict(zip([i for i in range(adj.shape[0])],node_labels[:adj.shape[0]]))
    nx.set_node_attributes(G,label_dict,name='label')
    edge_label_dict = dict()
    # print(adj.shape[0],adj.shape[1])
    for i in range(adj.shape[0]):
        for j in range(i+1,adj.shape[1]):
            if adj[i,j]==1:
                edge_label_dict[(i,j)] = lab_adj[i,j]
    nx.set_edge_attributes(G,edge_label_dict,'label')
    return G

def save_graph_list(G_list, fname):
    with open(fname, "wb") as f:
        pickle.dump(G_list, f)

def train_rnn_epoch(epoch, args, rnn, output, node_mlp, edge_mlp,data_loader,
                    optimizer_rnn, optimizer_output, optimizer_node_label_mlp, 
                    optimizer_edge_label_mlp,scheduler_rnn, scheduler_output,
                    scheduler_node_label_mlp, scheduler_edge_label_mlp):
    rnn.train()
    output.train()
    node_mlp.train()
    edge_mlp.train()
    criteron = nn.CrossEntropyLoss().cuda()
    activ = nn.Sigmoid()
    activ_2 = nn.Softmax(dim=1)
    loss_sum = 0
    for batch_idx, data in enumerate(data_loader):
        rnn.zero_grad()
        output.zero_grad()
        node_mlp.zero_grad()
        edge_mlp.zero_grad()
        x_unsorted = data['x'].float()
        y_unsorted = data['y'].float()
        lab_unsorted = data['lab'] # 2D matrix
        y_len_unsorted = data['len']
        y_len_max = max(y_len_unsorted)
        x_unsorted = x_unsorted[:, 0:y_len_max, :]
        y_unsorted = y_unsorted[:, 0:y_len_max, :]
        edge_unsorted = torch.tensor(y_unsorted,requires_grad=True)
        y_unsorted[y_unsorted!=0] = 1
        lab_unsorted = lab_unsorted[:,0:y_len_max]
        # initialize lstm hidden state according to batch size
        rnn.hidden = rnn.init_hidden(batch_size=x_unsorted.size(0))
        # output.hidden = output.init_hidden(batch_size=x_unsorted.size(0)*x_unsorted.size(1))

        # sort input
        
        y_len,sort_index = torch.sort(y_len_unsorted,0,descending=True)
        y_len = y_len.numpy().tolist()
        x = torch.index_select(x_unsorted,0,sort_index)
        y = torch.index_select(y_unsorted,0,sort_index)
        edge_sorted = torch.index_select(edge_unsorted,0,sort_index)
        lab_sorted = torch.index_select(lab_unsorted,0,sort_index)
        
        # input, output for output rnn module
        # a smart use of pytorch builtin function: pack variable--b1_l1,b2_l1,...,b1_l2,b2_l2,...
        y_reshape = pack_padded_sequence(y,y_len,batch_first=True).data
        edge_reshape = pack_padded_sequence(edge_sorted,y_len,batch_first=True).data
        # reverse y_reshape, so that their lengths are sorted, add dimension
        idx = [i for i in range(y_reshape.size(0)-1, -1, -1)]
        idx = torch.LongTensor(idx)
        y_reshape = y_reshape.index_select(0, idx)
        y_reshape = y_reshape.view(y_reshape.size(0),y_reshape.size(1),1)

        edge_reshape = edge_reshape.index_select(0, idx)
        edge_reshape = edge_reshape.view(edge_reshape.size(0),edge_reshape.size(1),1)

        output_x = torch.cat((torch.ones(y_reshape.size(0),1,1),y_reshape[:,0:-1,0:1]),dim=1)
        output_y = y_reshape
        # batch size for output module: sum(y_len)
        output_y_len = []
        output_y_len_bin = np.bincount(np.array(y_len))
        for i in range(len(output_y_len_bin)-1,0,-1):
            count_temp = np.sum(output_y_len_bin[i:]) # count how many y_len is above i
            output_y_len.extend([min(i,y.size(2))]*count_temp) # put them in output_y_len; max value should not exceed y.size(2)
        # pack into variable
        x = Variable(x).cuda()
        y = Variable(y).cuda()
        output_x = Variable(output_x).cuda()
        output_y = Variable(output_y).cuda()
        edge_reshape = Variable(edge_reshape).cuda()
        # print(output_y_len)
        # print('len',len(output_y_len))
        # print('y',y.size())
        # print('output_y',output_y.size())


        # if using ground truth to train
        h,graph_level_hidden = rnn(x, pack=True, input_len=y_len, get_hidden=True)
        graph_level_hidden = graph_level_hidden.clone().detach()
        # graph_level_hidden.grad.data.zero_()
        # print(graph_level_hidden)
        node_label_input = pack_padded_sequence(graph_level_hidden,y_len,batch_first=True).data
        # output_mark = lab_sorted.view(-1)
        node_output_markers = pack_padded_sequence(lab_sorted,y_len,batch_first=True).data
        node_label_input = Variable(node_label_input).cuda()
        node_output_markers = Variable(node_output_markers).cuda()
        node_label_predict = node_mlp(node_label_input)
        h = pack_padded_sequence(h,y_len,batch_first=True).data # get packed hidden vector
        # reverse h
        idx = [i for i in range(h.size(0) - 1, -1, -1)]
        idx = Variable(torch.LongTensor(idx)).cuda()
        h = h.index_select(0, idx)
        hidden_null = Variable(torch.zeros(args.num_layers-1, h.size(0), h.size(1))).cuda()
        output.hidden = torch.cat((h.view(1,h.size(0),h.size(1)),hidden_null),dim=0) # num_layers, batch_size, hidden_size
        # print(' size of output_hidden is {}'.format(output.hidden.size()))
        y_pred,edge_level_hidden = output(output_x, pack=True, input_len=output_y_len,get_hidden = True)
        edge_level_hidden = edge_level_hidden.clone().detach()
        edge_label_input = pack_padded_sequence(edge_level_hidden,output_y_len,batch_first=True).data
        edge_output_markers = pack_padded_sequence(edge_reshape,output_y_len,batch_first=True).data
        edge_output_markers = edge_output_markers.view(-1)
        edge_label_input = Variable(edge_label_input).cuda()
        edge_output_markers = Variable(edge_output_markers).cuda()
        edge_label_predict = edge_mlp(edge_label_input)
        y_pred = F.sigmoid(y_pred)
        # clean
        y_pred = pack_padded_sequence(y_pred, output_y_len, batch_first=True)
        y_pred = pad_packed_sequence(y_pred, batch_first=True)[0]
        output_y = pack_padded_sequence(output_y,output_y_len,batch_first=True)
        output_y = pad_packed_sequence(output_y,batch_first=True)[0]
        # use cross entropy loss
        loss = binary_cross_entropy_weight(y_pred, output_y)
        loss.backward()

        # for node labels loss
        node_label_predict = activ(node_label_predict)
        loss_node_label = criteron(node_label_predict,node_output_markers)
        loss_node_label.backward()
        
        # for edge labels loss
        edge_label_predict = activ_2(edge_label_predict)
        # k = (torch.Tensor(hello))
        # index=k.nonzero().view(-1)
        # u = k.index_select(0,index)
        indexes = edge_output_markers.nonzero().detach().view(-1)
        edge_output_markers = edge_output_markers.index_select(0,indexes)
        edge_output_markers = edge_output_markers.long()
        edge_label_predict = edge_label_predict.index_select(0,indexes)
        # print(edge_label_predict.size())
        # print(edge_output_markers.size())
        # print(edge_output_markers[:100])
        # exit()
        loss_edge_label = criteron(edge_label_predict,edge_output_markers)
        loss_edge_label.backward()

        # update deterministic and lstm


        optimizer_output.step()
        optimizer_rnn.step()
        optimizer_node_label_mlp.step()
        optimizer_edge_label_mlp
        scheduler_output.step()
        scheduler_rnn.step()
        scheduler_node_label_mlp.step()
        scheduler_edge_label_mlp.step()


        if epoch % args.epochs_log==0 and batch_idx==0: # only output first batch's statistics
            print('Epoch: {}/{}, train loss: {:.6f}, train label loss: {:.6f}, train edge loss: {:.6f}, graph type: {}, num_layer: {}, hidden: {}'.format(
                epoch, args.epochs,loss.data,loss_node_label.data,loss_edge_label.data,args.graph_type, args.num_layers, args.hidden_size_rnn))

        # logging
        # print(shape(loss.data))
        # log_value('loss_'+args.fname, loss.data, epoch*args.batch_ratio+batch_idx)
        feature_dim = y.size(1)*y.size(2)
        loss_sum += loss.data*feature_dim
    return loss_sum/(batch_idx+1)

def decode_label_adj(adj_output,label_output):
    '''
        recover to adj from adj_output
        note: here adj_output have shape (n-1)*m
    '''
    max_prev_node = adj_output.shape[1]
    adj = np.zeros((adj_output.shape[0], adj_output.shape[0]))
    lab_adj = np.zeros((label_output.shape[0],adj_output.shape[0]))
    for i in range(adj_output.shape[0]):
        input_start = max(0, i - max_prev_node + 1)
        input_end = i + 1
        output_start = max_prev_node + max(0, i - max_prev_node + 1) - (i + 1)
        output_end = max_prev_node
        adj[i, input_start:input_end] = adj_output[i,::-1][output_start:output_end] # reverse order
        lab_adj[i, input_start:input_end] = label_output[i,::-1][output_start:output_end] # reverse order
    adj_full = np.zeros((adj_output.shape[0]+1, adj_output.shape[0]+1))
    lab_adj_full = np.zeros((adj_output.shape[0]+1, adj_output.shape[0]+1))
    n = adj_full.shape[0]
    adj_full[1:n, 0:n-1] = np.tril(adj, 0)
    lab_adj_full[1:n, 0:n-1] = np.tril(lab_adj, 0)
    adj_full = adj_full + adj_full.T
    lab_adj_full = lab_adj_full + lab_adj_full.T
    return adj_full, lab_adj_full


def test_rnn_epoch(epoch, args, rnn, output, node_mlp,edge_mlp, test_batch_size=16):
    rnn.hidden = rnn.init_hidden(test_batch_size)
    rnn.eval()
    output.eval()
    node_mlp.eval()
    edge_mlp.eval()
    activ = nn.Sigmoid()
    # generate graphs
    max_num_node = int(args.max_num_node)
    y_pred_long = Variable(torch.zeros(test_batch_size, max_num_node, args.max_prev_node)).cuda() # discrete prediction
    edge_pred_long = Variable(torch.zeros(test_batch_size,max_num_node,args.max_prev_node)).cuda() # edge prediction
    x_step = Variable(torch.ones(test_batch_size,1,args.max_prev_node)).cuda()
    lab_step = Variable(torch.zeros(test_batch_size,args.max_num_node))
    for i in range(max_num_node):
        h,graph_hidden = rnn(x_step,get_hidden=True)
        # output.hidden = h.permute(1,0,2)
        graph_hidden = graph_hidden.view(-1,graph_hidden.size(2))
        test_labels = node_mlp(graph_hidden)
        test_labels = test_labels[:,1:]
        test_labels = activ(test_labels)
        categorical_labels = torch.distributions.Categorical(test_labels)
        categorical_values = categorical_labels.sample()
        categorical_values = categorical_values + 1
        # print(categorical_values.size())
        lab_step[:,i:i+1] = categorical_values.view(-1,1)
        # print(test_batch_size)
        # exit()
        hidden_null = Variable(torch.zeros(args.num_layers - 1, h.size(0), h.size(2))).cuda()
        output.hidden = torch.cat((h.permute(1,0,2), hidden_null),
                                  dim=0)  # num_layers, batch_size, hidden_size
        x_step = Variable(torch.zeros(test_batch_size,1,args.max_prev_node)).cuda()
        edge_step = Variable(torch.zeros(test_batch_size,1,args.max_prev_node)).cuda()
        output_x_step = Variable(torch.ones(test_batch_size,1,1)).cuda()
        for j in range(min(args.max_prev_node,i+1)):
            output_y_pred_step ,edge_hidden = output(output_x_step,get_hidden=True)
            output_x_step = sample_sigmoid(output_y_pred_step, sample=True, sample_time=1)
            indexes = output_x_step.nonzero()
            edge_hidden = edge_hidden.view(-1,edge_hidden.size(2))
            # print(indexes)
            flag = indexes.size(0)
            if flag!=0:
                indexes = indexes[:,0]
                edge_hidden = edge_hidden.index_select(0,indexes)
                edge_output = edge_mlp(edge_hidden)
                edge_output = edge_output[:,1:]
                edge_output = activ(edge_output)
                categorical_edge_labels = torch.distributions.Categorical(edge_output)
                categorical_edge_values = categorical_edge_labels.sample()
                categorical_edge_values = categorical_edge_values + 1 
                # categorical_edge_values = categorical_edge_values.view(test_batch_size,1,1)     
                setup_edge_values = Variable(torch.zeros(test_batch_size)).cuda()
                for item in range(len(indexes)):
                    setup_edge_values[indexes[item]] = categorical_edge_values[item]
                setup_edge_values = setup_edge_values.view(test_batch_size,1,1) 
                edge_step[:,:,j:j+1] = setup_edge_values
            x_step[:,:,j:j+1] = output_x_step
            output.hidden = Variable(output.hidden.data).cuda()
        y_pred_long[:, i:i + 1, :] = x_step
        edge_pred_long[:, i:i + 1, :] = edge_step
        rnn.hidden = Variable(rnn.hidden.data).cuda()
    y_pred_long_data = y_pred_long.data.long()

    # save graphs as pickle
    G_pred_list = []
    for i in range(test_batch_size):
        adj_pred,lab_pred = decode_label_adj(y_pred_long_data[i].cpu().numpy(),edge_pred_long[i].cpu().numpy())
        G_pred = get_label_graph(adj_pred,lab_step[i].cpu().numpy().tolist(),lab_pred) # get a graph from zero-padded adj
        G_pred_list.append(G_pred)

    return G_pred_list




def train_rnn_forward_epoch(epoch, args, rnn,output, data_loader):
    rnn.train()
    output.train()
    loss_sum = 0
    for batch_idx, data in enumerate(data_loader):
        rnn.zero_grad()
        output.zero_grad()
        x_unsorted = data['x'].float()
        y_unsorted = data['y'].float()
        y_len_unsorted = data['len']
        y_len_max = max(y_len_unsorted)
        x_unsorted = x_unsorted[:, 0:y_len_max, :]
        y_unsorted = y_unsorted[:, 0:y_len_max, :]
        # initialize lstm hidden state according to batch size
        rnn.hidden = rnn.init_hidden(batch_size=x_unsorted.size(0))
        # output.hidden = output.init_hidden(batch_size=x_unsorted.size(0)*x_unsorted.size(1))

        # sort input
        y_len,sort_index = torch.sort(y_len_unsorted,0,descending=True)
        y_len = y_len.numpy().tolist()
        x = torch.index_select(x_unsorted,0,sort_index)
        y = torch.index_select(y_unsorted,0,sort_index)

        # input, output for output rnn module
        # a smart use of pytorch builtin function: pack variable--b1_l1,b2_l1,...,b1_l2,b2_l2,...
        y_reshape = pack_padded_sequence(y,y_len,batch_first=True).data
        # reverse y_reshape, so that their lengths are sorted, add dimension
        idx = [i for i in range(y_reshape.size(0)-1, -1, -1)]
        idx = torch.LongTensor(idx)
        y_reshape = y_reshape.index_select(0, idx)
        y_reshape = y_reshape.view(y_reshape.size(0),y_reshape.size(1),1)

        output_x = torch.cat((torch.ones(y_reshape.size(0),1,1),y_reshape[:,0:-1,0:1]),dim=1)
        output_y = y_reshape
        # batch size for output module: sum(y_len)
        output_y_len = []
        output_y_len_bin = np.bincount(np.array(y_len))
        for i in range(len(output_y_len_bin)-1,0,-1):
            count_temp = np.sum(output_y_len_bin[i:]) # count how many y_len is above i
            output_y_len.extend([min(i,y.size(2))]*count_temp) # put them in output_y_len; max value should not exceed y.size(2)
        # pack into variable
        x = Variable(x).cuda()
        y = Variable(y).cuda()
        output_x = Variable(output_x).cuda()
        output_y = Variable(output_y).cuda()

        # if using ground truth to train
        h = rnn(x, pack=True, input_len=y_len)
        h = pack_padded_sequence(h,y_len,batch_first=True).data # get packed hidden vector
        # reverse h
        idx = [i for i in range(h.size(0) - 1, -1, -1)]
        idx = Variable(torch.LongTensor(idx)).cuda()
        h = h.index_select(0, idx)
        hidden_null = Variable(torch.zeros(args.num_layers-1, h.size(0), h.size(1))).cuda()
        output.hidden = torch.cat((h.view(1,h.size(0),h.size(1)),hidden_null),dim=0) # num_layers, batch_size, hidden_size
        y_pred = output(output_x, pack=True, input_len=output_y_len)
        y_pred = F.sigmoid(y_pred)
        # clean
        y_pred = pack_padded_sequence(y_pred, output_y_len, batch_first=True)
        y_pred = pad_packed_sequence(y_pred, batch_first=True)[0]
        output_y = pack_padded_sequence(output_y,output_y_len,batch_first=True)
        output_y = pad_packed_sequence(output_y,batch_first=True)[0]
        # use cross entropy loss
        loss = binary_cross_entropy_weight(y_pred, output_y)


        if epoch % args.epochs_log==0 and batch_idx==0: # only output first batch's statistics
            print('Epoch: {}/{}, train loss: {:.6f}, graph type: {}, num_layer: {}, hidden: {}'.format(
                epoch, args.epochs,loss.data[0], args.graph_type, args.num_layers, args.hidden_size_rnn))

        # logging
        # log_value('loss_'+args.fname, loss.data[0], epoch*args.batch_ratio+batch_idx)
        # print(y_pred.size())
        feature_dim = y_pred.size(0)*y_pred.size(1)
        loss_sum += loss.data[0]*feature_dim/y.size(0)
    return loss_sum/(batch_idx+1)


########### train function for LSTM + VAE
def train(args, dataset_train, rnn, output, node_mlp, edge_mlp):
    # check if load existing model
    if args.load:
        fname = args.model_save_path + args.fname + 'lstm_' + str(args.load_epoch) + '.dat'
        rnn.load_state_dict(torch.load(fname))
        fname = args.model_save_path + args.fname + 'output_' + str(args.load_epoch) + '.dat'
        output.load_state_dict(torch.load(fname))
        fname = args.model_save_path + args.fname + 'node_label_mlp_' + str(args.load_epoch) + '.dat'
        node_mlp.load_state_dict(torch.load(fname))
        fname = args.model_save_path + args.fname + 'edge_label_mlp_' + str(args.load_epoch) + '.dat'
        edge_mlp.load_state_dict(torch.load(fname))
        args.lr = 0.00001
        epoch = args.load_epoch
        print('model loaded!, lr: {}'.format(args.lr))
    else:
        epoch = 1

    # initialize optimizer
    optimizer_rnn = optim.Adam(list(rnn.parameters()), lr=args.lr)
    optimizer_output = optim.Adam(list(output.parameters()), lr=args.lr)
    optimizer_node_label_mlp = optim.Adam(list(node_mlp.parameters()),lr=args.lr_mlp)
    optimizer_edge_label_mlp = optim.Adam(list(edge_mlp.parameters()),lr=args.lr_mlp)
    scheduler_rnn = MultiStepLR(optimizer_rnn, milestones=args.milestones, gamma=args.lr_rate)
    scheduler_output = MultiStepLR(optimizer_output, milestones=args.milestones, gamma=args.lr_rate)
    scheduler_node_label_mlp = MultiStepLR(optimizer_node_label_mlp,milestones=args.milestones, gamma=args.lr_rate)
    scheduler_edge_label_mlp = MultiStepLR(optimizer_edge_label_mlp,milestones=args.milestones, gamma=args.lr_rate)
    # start main loop
    time_all = np.zeros(args.epochs)
    while epoch<=args.epochs:
        time_start = tm.time()
        # train
        train_rnn_epoch(epoch, args, rnn, output,node_mlp,edge_mlp,dataset_train,
                        optimizer_rnn, optimizer_output,optimizer_node_label_mlp,
                        optimizer_edge_label_mlp,scheduler_rnn, scheduler_output,
                        scheduler_node_label_mlp,scheduler_edge_label_mlp)
        time_end = tm.time()
        time_all[epoch - 1] = time_end - time_start
        # test
        if epoch % args.epochs_test == 0 and epoch>=args.epochs_test_start:
            for sample_time in range(1,4):
                G_pred = []
                while len(G_pred)<args.test_total_size:
                    G_pred_step = test_rnn_epoch(epoch, args, rnn, output, node_mlp, edge_mlp,test_batch_size=args.test_batch_size)
                    G_pred.extend(G_pred_step)
                # save graphs
                fname = args.graph_save_path + args.fname_pred + str(epoch) +'_'+str(sample_time) + '.dat'
                save_graph_list(G_pred, fname)
                if 'GraphRNN_RNN' in args.note:
                    break
            print('test done, graphs saved')


        # save model checkpoint
        if args.save:
            if epoch % args.epochs_save == 0:
                fname = args.model_save_path + args.fname + 'lstm_' + str(epoch) + '.dat'
                torch.save(rnn.state_dict(), fname)
                fname = args.model_save_path + args.fname + 'output_' + str(epoch) + '.dat'
                torch.save(output.state_dict(), fname)
                fname = args.model_save_path + args.fname + 'node_label_mlp_' + str(epoch) + '.dat'
                torch.save(node_mlp.state_dict(), fname)
                fname = args.model_save_path + args.fname + 'edge_label_mlp_' + str(epoch) + '.dat'
                torch.save(edge_mlp.state_dict(), fname)
        epoch += 1
    np.save(args.timing_save_path+args.fname,time_all)


########### for graph completion task
def train_graph_completion(args, dataset_test, rnn, output):
    fname = args.model_save_path + args.fname + 'lstm_' + str(args.load_epoch) + '.dat'
    rnn.load_state_dict(torch.load(fname))
    fname = args.model_save_path + args.fname + 'output_' + str(args.load_epoch) + '.dat'
    output.load_state_dict(torch.load(fname))

    epoch = args.load_epoch
    print('model loaded!, epoch: {}'.format(args.load_epoch))

    for sample_time in range(1,4):
        if 'GraphRNN_MLP' in args.note:
            G_pred = test_mlp_partial_simple_epoch(epoch, args, rnn, output, dataset_test,sample_time=sample_time)
        if 'GraphRNN_VAE' in args.note:
            G_pred = test_vae_partial_epoch(epoch, args, rnn, output, dataset_test,sample_time=sample_time)
        # save graphs
        fname = args.graph_save_path + args.fname_pred + str(epoch) +'_'+str(sample_time) + 'graph_completion.dat'
        save_graph_list(G_pred, fname)
    print('graph completion done, graphs saved')
