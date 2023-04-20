from .NetOptions import NetOptions

import torch.utils.data as data
import os
import shutil

class BaseNetTrainer():
    '''
    This class is used to setup a bunch of the stuff to run the actual training and testing on
    '''
    Options : NetOptions = None;
    DebugMode : bool = False;

    def __init__(self, options : NetOptions, dataset: data.Dataset, debugMode : bool = False):
        # Setup the output experiment path
        self.Options = options;
        self.DebugMode = debugMode;
        if os.path.exists(self.Options.experiment_root):
            shutil.rmtree(self.Options.experiment_root)
            os.makedirs(self.Options.experiment_root)
        else:
            os.makedirs(self.Options.experiment_root)

    def Train(self) -> float:
        raise Exception("Not Implemented!")

    # The total accuracy, and the pr. class accuracy (if available)
    def Test(self) -> tuple[float,dict]:
        raise Exception("Not Implemented!")
