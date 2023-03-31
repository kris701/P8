from .NetOptions import NetOptions

import torch.utils.data as data
import os
import shutil

class BaseNetTrainer():
    '''
    This class is used to setup a bunch of the stuff to run the actual training and testing on
    '''
    Options : NetOptions = None;

    def __init__(self, options : NetOptions, dataset: data.Dataset):
        # Setup the output experiment path
        self.Options = options;
        if os.path.exists(self.Options.experiment_root):
            shutil.rmtree(self.Options.experiment_root)
            os.makedirs(self.Options.experiment_root)
        else:
            os.makedirs(self.Options.experiment_root)

    def Train(self) -> float:
        raise Exception("Not Implemented!")

    def Test(self) -> float:
        raise Exception("Not Implemented!")
