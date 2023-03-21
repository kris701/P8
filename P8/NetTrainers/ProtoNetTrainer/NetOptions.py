import torch.nn as nn
from .Backbones.slprotonet import slProtoNet

class NetOptions():
    # path to dataset
    dataset_root : str = "formated";
    # root where to store models, losses and accuracies
    experiment_root : str = "output";
    # number of epochs to train for
    train_epochs : int = 10;
    # number of epochs to test for
    test_epochs : int = 10;
    # learning rate for the model
    learning_rate : float = 0.001;
    # StepLR learning rate scheduler step
    lr_scheduler_step : int = 20;
    # StepLR learning rate scheduler gamma
    lr_scheduler_gamma : float = 0.5;
    # number of episodes per epoch
    iterations : int = 100;

    # if we should load the train set or not
    load_train_set : bool = True;
    # number of random classes per episode for training
    classes_per_it_tr : int = 60;
    # number of samples per class to use as support for training
    num_support_tr : int = 5;
    # number of samples per class to use as query for training
    num_query_tr : int = 5;

    # if we should load the val set or not
    load_val_set : bool = True;
    # number of random classes per episode for validation
    classes_per_it_val : int = 5;
    # number of samples per class to use as support for validation
    num_support_val : int = 5;
    # number of samples per class to use as query for validation
    num_query_val : int = 5;

    # if we should load the test set or not
    load_test_set : bool = True;
    # number of random classes per episode for test
    classes_per_it_test : int = 5;
    # number of samples per class to use as support for test
    num_support_test : int = 5;
    # number of samples per class to use as query for test
    num_query_test : int = 15;

    # input for the manual seeds initializations
    manual_seed : int = 7;
    # enables cuda
    cuda : bool = True;
    # option to select other cuda devices to run on
    cudaDevice : str = "cuda:0";

    # input dimensions
    x_dim : int = 1;
    # hidden dimensions
    hid_dim : int = 8;
    # output dimensions
    z_dim = int = 8;
    # Backbone protonet to use
    backbone : nn.Module = slProtoNet;
