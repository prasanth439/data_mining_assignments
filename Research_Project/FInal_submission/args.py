
### program configuration
class Args():
    # def __init__(self,graph_type,input_file,epocs=1000):
    def __init__(self,graph_type=None,input_file=None,
                epocs=None,batch_size=None,
                batch_ratio=None,lr_mlp_=None,
                hidden_size_rnn=None,hidden_size_rnn_output=None,
                embedding_size_rnn=None,embedding_size_rnn_output=None,
                embedding_size_output=None):
        self.note = 'GraphRNN_RNN'
        self.graph_type = graph_type
        #ours
        self.input_file = input_file
        # if none, then auto calculate
        self.max_num_node = None # max number of nodes in a graph
        self.max_prev_node = 100 # max previous node that looks back
        self.max_node_labels = None
        self.max_edge_labels = None
        self.edge_mapping = None
        self.node_mapping = None
        self.node_rev_map = None
        self.edge_rev_map = None
        ### network config
        ## GraphRNN

        self.parameter_shrink = 1
        self.hidden_size_rnn = hidden_size_rnn # hidden size for main RNN
        self.hidden_size_rnn_output = hidden_size_rnn_output # hidden size for output RNN
        self.embedding_size_rnn = embedding_size_rnn # the size for LSTM input
        self.embedding_size_rnn_output = embedding_size_rnn_output # the embedding size for output rnn
        self.embedding_size_output = embedding_size_output # the embedding size for output (VAE/MLP)

        self.batch_size = batch_size # normal: 32, and the rest should be changed accordingly
        self.test_batch_size = 32
        self.test_total_size = 2560
        self.num_layers = 4

        ### training config
        self.num_workers = 0 # num workers to load data, default 4
        self.batch_ratio = batch_ratio # how many batches of samples per epoch, default 32, e.g., 1 epoch = 32 batches
        self.epochs = epocs # now one epoch means self.batch_ratio x batch_size
        self.epochs_test_start = 2
        self.epochs_test = 2
        self.epochs_log = 2
        self.epochs_save = 50

        self.lr = 0.003
        self.lr_mlp = lr_mlp_
        self.milestones = [200, 400, 600]
        self.lr_rate = 0.3

        self.sample_time = 2 # sample time in each time step, when validating

        ### output config
        # self.dir_input = "/dfs/scratch0/jiaxuany0/"
        self.dir_input = "./"
        self.model_save_path = self.dir_input+'model_save/' # only for nll evaluation
        self.graph_save_path = self.dir_input+'graphs/'
        self.figure_save_path = self.dir_input+'figures/'
        self.timing_save_path = self.dir_input+'timing/'
        self.figure_prediction_save_path = self.dir_input+'figures_prediction/'


        self.load = False # if load model, default lr is very low
        self.load_epoch = 3000
        self.save = True


        ### baseline config
        # self.generator_baseline = 'Gnp'
        self.generator_baseline = 'BA'

        # self.metric_baseline = 'general'
        # self.metric_baseline = 'degree'
        self.metric_baseline = 'clustering'


        ### filenames to save intemediate and final outputs
        self.fname = self.note + '_' + self.graph_type + '_' + str(self.num_layers) + '_' + str(self.hidden_size_rnn) + '_'
        self.fname_pred = self.note+'_'+self.graph_type+'_'+str(self.num_layers)+'_'+ str(self.hidden_size_rnn)+'_pred_'
        self.fname_train = self.note+'_'+self.graph_type+'_'+str(self.num_layers)+'_'+ str(self.hidden_size_rnn)+'_train_'
        self.fname_test = self.note + '_' + self.graph_type + '_' + str(self.num_layers) + '_' + str(self.hidden_size_rnn) + '_test_'
        self.fname_baseline = self.graph_save_path + self.graph_type + self.generator_baseline+'_'+self.metric_baseline

