from train import *
from graph_process import convert_to_networkGraphs

if __name__ == '__main__':
    # All necessary arguments are defined in args.py
    args = Args()

    time = strftime("%Y-%m-%d %H:%M:%S", gmtime())

    graphs, max_node_labels, max_edge_labels,node_mapping, edge_mapping = convert_to_networkGraphs(args.input_file)
    args.max_node_labels = max_node_labels
    args.max_edge_labels = max_edge_labels
    args.node_mapping = node_mapping
    args.edge_mapping = edge_mapping
    if not os.path.isdir(args.model_save_path):
        os.makedirs(args.model_save_path)
    if not os.path.isdir(args.graph_save_path):
        os.makedirs(args.graph_save_path)
    if not os.path.isdir(args.figure_save_path):
        os.makedirs(args.figure_save_path)
    if not os.path.isdir(args.timing_save_path):
        os.makedirs(args.timing_save_path)
    if not os.path.isdir(args.figure_prediction_save_path):
        os.makedirs(args.figure_prediction_save_path)

    # split datasets
    random.seed(123)
    shuffle(graphs)
    graphs_len = len(graphs)
    graphs_test = graphs[int(0.8 * graphs_len):]
    graphs_train = graphs[0:int(0.8*graphs_len)]
    graphs_validate = graphs[0:int(0.2*graphs_len)]


    graph_validate_len = 0
    for graph in graphs_validate:
        graph_validate_len += graph.number_of_nodes()
    graph_validate_len /= len(graphs_validate)
    print('graph_validate_len', graph_validate_len)

    graph_test_len = 0
    for graph in graphs_test:
        graph_test_len += graph.number_of_nodes()
    graph_test_len /= len(graphs_test)
    print('graph_test_len', graph_test_len)



    args.max_num_node = max([graphs[i].number_of_nodes() for i in range(len(graphs))])
    max_num_edge = max([graphs[i].number_of_edges() for i in range(len(graphs))])
    min_num_edge = min([graphs[i].number_of_edges() for i in range(len(graphs))])

    # args.max_num_node = 2000
    # show graphs statistics
    print('total graph num: {}, training set: {}'.format(len(graphs),len(graphs_train)))
    print('max number node: {}'.format(args.max_num_node))
    print('max/min number edge: {}; {}'.format(max_num_edge,min_num_edge))
    print('max previous node: {}'.format(args.max_prev_node))
    print('max node labels: {}'.format(args.max_node_labels))
    print('max edge labels: {}'.format(args.max_edge_labels))

    # save ground truth graphs
    ## To get train and test set, after loading you need to manually slice
    save_graph_list(graphs, args.graph_save_path + args.fname_train + '0.dat')
    save_graph_list(graphs, args.graph_save_path + args.fname_test + '0.dat')
    args.max_prev_node = 23
    dataset = Graph_sequence_sampler_pytorch(graphs_train,max_prev_node=args.max_prev_node,max_num_node=args.max_num_node)
    sample_strategy = torch.utils.data.sampler.WeightedRandomSampler([1.0 / len(dataset) for i in range(len(dataset))],
                                                                        num_samples=args.batch_size*args.batch_ratio, replacement=True)
    dataset_loader = torch.utils.data.DataLoader(dataset, batch_size=args.batch_size, num_workers=args.num_workers,
                                                sampler=sample_strategy)
    if args.max_prev_node == None:
        args.max_prev_node = dataset.max_prev_node
    ### model initialization
    ## Graph RNN VAE model
    # lstm = LSTM_plain(input_size=args.max_prev_node, embedding_size=args.embedding_size_lstm,
    #                   hidden_size=args.hidden_size, num_layers=args.num_layers).cuda()

    rnn = GRU_plain(input_size=args.max_prev_node, embedding_size=args.embedding_size_rnn,
                    hidden_size=args.hidden_size_rnn, num_layers=args.num_layers, has_input=True,
                    has_output=True, output_size=args.hidden_size_rnn_output).cuda()
    output = GRU_plain(input_size=1, embedding_size=args.embedding_size_rnn_output,
                        hidden_size=args.hidden_size_rnn_output, num_layers=args.num_layers, has_input=True,
                        has_output=True, output_size=1).cuda()
    node_label_layers = [args.hidden_size_rnn,256,128,64,args.max_node_labels]
    node_mlp = LabelMLP(node_label_layers).cuda()
    edge_label_layers = [args.hidden_size_rnn_output,128,64,64,args.max_edge_labels]
    edge_mlp = LabelMLP(edge_label_layers).cuda()
    ### start training
    train(args, dataset_loader, rnn, output,node_mlp,edge_mlp)
