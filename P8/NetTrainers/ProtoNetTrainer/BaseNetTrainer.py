from .prototypical_batch_sampler import PrototypicalBatchSampler
from .NetOptions import NetOptions
from .DataloaderVerifier import DataLoaderVerifier

import torch.utils.data as data
import numpy as np
import torch
import os
import torch.nn as nn
import shutil

class BaseNetTrainer():
    '''
    This class is used to setup a bunch of the stuff to run the actual training and testing on
    '''
    Options : NetOptions = None;

    Dataset : data.Dataset = None;
    TrainDataloader : data.DataLoader = None;
    ValDataloader : data.DataLoader = None;
    TestDataloader : data.DataLoader = None;

    _isTrained : bool = False;
    BestModel : nn.Module = None;

    def __init__(self, options : NetOptions, dataset : data.Dataset):
        # Setup the output experiment path
        self.Options = options;
        if os.path.exists(self.Options.experiment_root):
            shutil.rmtree(self.Options.experiment_root)
            os.makedirs(self.Options.experiment_root)
        else:
            os.makedirs(self.Options.experiment_root)

        # Check if CUDA is available 
        if torch.cuda.is_available() and not self.Options.cuda:
            print("WARNING: You have a CUDA device, so you should probably run with --cuda")

        # Initialize torch seeds
        self._init_seed()

        # Setup the dataloaders for the dataset 
        self.Dataset = dataset;
        self.TrainDataloader = self._init_dataloader('train', self.Dataset)
        self.ValDataloader = self._init_dataloader('val', self.Dataset)
        self.TestDataloader = self._init_dataloader('test', self.Dataset)

        # Verify that there are no overlap between the dataloaders.
        DataLoaderVerifier.Verify([self.ValDataloader, self.TrainDataloader, self.TestDataloader])

    def _getDevice(self) -> str:
        '''
        Gets the device for Torch to run on.
        This will either be the CUDA device specified in the Options (if CUDA is enabled there too), or just the CPU device
        '''
        if torch.cuda.is_available() and self.Options.cuda:
            return self.Options.cudaDevice;
        else:
            return "cpu";

    def _init_seed(self) -> None:
        '''
        Initialized seeds for 'random' and 'torch' classes
        '''
        torch.cuda.cudnn_enabled = False
        np.random.seed(self.Options.manual_seed)
        torch.manual_seed(self.Options.manual_seed)
        torch.cuda.manual_seed(self.Options.manual_seed)

    def _init_sampler(self, labels : list, mode : str) -> PrototypicalBatchSampler:
        '''
        Initialized a data sampler, based on a set of labels and what "mode" it should be in.
        The available modes are 'train', 'val' and 'test'.
        Each of these have a corresponding set of options in the Options class
        '''
        if 'train' == mode:
            classes_per_it = self.Options.classes_per_it_tr
            num_samples = self.Options.num_support_tr + self.Options.num_query_tr
        elif 'val' == mode:
            classes_per_it = self.Options.classes_per_it_val
            num_samples = self.Options.num_support_val + self.Options.num_query_val
        elif 'test' == mode:
            classes_per_it = self.Options.classes_per_it_test
            num_samples = self.Options.num_support_test + self.Options.num_query_test
        else:
            raise Exception("Invalid 'mode' for the sampler!")

        return PrototypicalBatchSampler(labels=labels,
                                        classes_per_it=classes_per_it,
                                        num_samples=num_samples,
                                        iterations=self.Options.iterations)

    def _init_dataloader(self, mode : str, dataset : data.Dataset) -> data.DataLoader:
        '''
        Initialized a DataLoader instance, based on a mode and a dataset.
        '''
        dataset_ = dataset(mode=mode, root=self.Options.dataset_root)
        sampler = self._init_sampler(dataset_.y, mode)
        dataloader = data.DataLoader(dataset_, batch_sampler=sampler)
        return dataloader

    def _init_protonet(self, protonet : nn.Module) -> nn.Module:
        '''
        Initialized a new instance of the protonet backbone that is specified in the Options
        '''
        device = self._getDevice()
        model = protonet(
            self.Options.x_dim,
            self.Options.hid_dim,
            self.Options.z_dim
            ).to(device)
        return model

    def _save_list_to_file(self, path : str, thelist : list):
        '''
        Saves a list of data into a given file.
        '''
        with open(path, 'w') as f:
            for item in thelist:
                f.write("%s\n" % item)

    def Train(self) -> float:
        raise Exception("Not Implemented!")

    def Test(self) -> float:
        raise Exception("Not Implemented!")