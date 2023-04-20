from from .prototypical_loss import prototypical_loss as loss_fn
from ..BaseNetTrainer import BaseNetTrainer
from ..NetOptions import NetOptions
from .DataloaderVerifier import DataLoaderVerifier
from .prototypical_batch_sampler import PrototypicalBatchSampler

import torch.utils.data as data
from tqdm import tqdm
import numpy as np
import torch
import os
import torch.nn as nn

class NetTrainer(BaseNetTrainer):
    
    Dataset : data.Dataset = None;
    TrainDataloader : data.DataLoader = None;
    ValDataloader : data.DataLoader = None;
    TestDataloader : data.DataLoader = None;

    _isTrained : bool = False;
    BestModel : nn.Module = None;

    def __init__(self, options: NetOptions, dataset: data.Dataset, debugMode : bool):
        super().__init__(options, dataset, debugMode)
        
        self.Dataset = dataset;

        # Check if CUDA is available 
        if torch.cuda.is_available() and not self.Options.cuda:
            print("WARNING: You have a CUDA device, so you should probably run with --cuda")

        # Initialize torch seeds
        self._init_seed()

        # Setup the dataloaders for the dataset 
        self.Dataset = dataset;
        if self.Options.load_train_set:
            self.TrainDataloader = self._init_dataloader('train', self.Dataset)
        if self.Options.load_val_set:
            self.ValDataloader = self._init_dataloader('val', self.Dataset)
        if self.Options.load_test_set:
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
        '''
        Trains and Validates the model.
        Settings for these are given in the Options class.
        '''
        model = self._init_protonet(self.Options.backbone)
        optim = torch.optim.Adam(
            params=model.parameters(),
            lr=self.Options.learning_rate)
        lr_scheduler = torch.optim.lr_scheduler.StepLR(
            optimizer=optim,
            gamma=self.Options.lr_scheduler_gamma,
            step_size=self.Options.lr_scheduler_step);
        res = self._train(
            tr_dataloader=self.TrainDataloader,
            val_dataloader=self.ValDataloader,
            model=model,
            optim=optim,
            lr_scheduler=lr_scheduler)
        best_state, best_acc, train_loss, train_acc, val_loss, val_acc = res
        
        model.load_state_dict(best_state)

        self.BestModel = model;
        self._isTrained = True;

        return best_acc

    def _train(self, tr_dataloader : data.DataLoader, model : nn.Module, optim : torch.optim.Optimizer, lr_scheduler : torch.optim.lr_scheduler._LRScheduler, val_dataloader : data.DataLoader = None):
        '''
        Trains and Validates the model.
        Settings for these are given in the Options class.
        '''
        device = self._getDevice()

        if val_dataloader is None:
            best_state = None
        train_loss = []
        train_acc = []
        val_loss = []
        val_acc = []
        best_acc = 0

        best_model_path = os.path.join(self.Options.experiment_root, 'best_model.pth')
        last_model_path = os.path.join(self.Options.experiment_root, 'last_model.pth')

        pbar1 = tqdm(range(self.Options.train_epochs), desc='Loading...', position=0, colour="red", disable=not self.DebugMode)
        for epoch in pbar1:
            pbar1.set_description('Epoch {}, (Best Acc: {:0.2f})'.format(epoch + 1, best_acc), refresh=True);

            if tr_dataloader is None:
                raise Exception("Cannot train without a train set dataloader!")

            # Train
            tr_iter = iter(tr_dataloader)
            model.train()
            pbar2 = tqdm(tr_iter, desc='   Initializing model...', leave=False, position=1, colour="green", disable=not self.DebugMode)
            for batch in pbar2:
                optim.zero_grad()
                x, y = batch
                x, y = x.to(device), y.to(device)
                model_output = model(x)
                loss, acc, _, _ = loss_fn(model_output, target=y, n_support=self.Options.num_support_tr)
                loss.backward()
                optim.step()
                train_loss.append(loss.item())
                train_acc.append(acc.item())
                avg_loss = np.mean(train_loss[-self.Options.iterations:])
                avg_acc = np.mean(train_acc[-self.Options.iterations:])
                pbar2.set_description('   Train Loss: {:0.2f}, Train Acc: {:0.2f}'.format(avg_loss, avg_acc), refresh=True);
            lr_scheduler.step()
            
            # Eval
            if val_dataloader is None:
                if avg_acc > best_acc:
                    torch.save(model.state_dict(), best_model_path)
                    best_acc = avg_acc
                    best_state = model.state_dict()
                continue
            val_iter = iter(val_dataloader)
            model.eval()
            pbar2 = tqdm(val_iter, desc='   Initializing model...', leave=False, position=1, colour="blue", disable=not self.DebugMode)
            for batch in pbar2:
                x, y = batch
                x, y = x.to(device), y.to(device)
                model_output = model(x)
                loss, acc = loss_fn(model_output, target=y,
                                    n_support=self.Options.num_support_val)
                val_loss.append(loss.item())
                val_acc.append(acc.item())
                avg_loss = np.mean(val_loss[-self.Options.iterations:])
                avg_acc = np.mean(val_acc[-self.Options.iterations:])
                pbar2.set_description('   Val Loss: {:0.2f}, Val Acc: {:0.2f}    '.format(avg_loss, avg_acc), refresh=True);
            if avg_acc >= best_acc:
                torch.save(model.state_dict(), best_model_path)
                best_acc = avg_acc
                best_state = model.state_dict()

        torch.save(model.state_dict(), last_model_path)

        if self.DebugMode is True:
            print("Outputting best model to '{}'".format(self.Options.experiment_root))
        for name in ['train_loss', 'train_acc', 'val_loss', 'val_acc']:
            self._save_list_to_file(os.path.join(self.Options.experiment_root,
                                           name + '.txt'), locals()[name])

        return best_state, best_acc, train_loss, train_acc, val_loss, val_acc

    def Test(self) -> tuple[float,dict]:
        '''
        Tests the best model with a set of new data.
        '''
        if self._isTrained == False:
            raise Exception("Model is not trained! Cannot test on it.")

        return self._test(
                test_dataloader=self.TestDataloader,
                model=self.BestModel)

    def _test(self, test_dataloader : data.DataLoader, model : nn.Module) -> tuple[float,dict]:
        '''
        Tests a model with a set of new data.
        '''
        if test_dataloader is None:
                raise Exception("Cannot test without a test set dataloader!")

        device = self._getDevice()
        avg_acc = list()
        class_total = {}
        class_acc = {}
        pbar1 = tqdm(range(self.Options.test_epochs), desc='Loading...', position=0, colour="red", disable=not self.DebugMode)
        for epoch in pbar1:
            pbar1.set_description('Epoch {}'.format(epoch + 1), refresh=True);
            test_iter = iter(test_dataloader)
            for batch in test_iter:
                x, y = batch
                x, y = x.to(device), y.to(device)
                model_output = model(x)
                _, acc, expected, actual = loss_fn(model_output, target=y,
                                 n_support=self.Options.num_support_test)

                for i in range(0,len(expected)):
                    if expected[i] not in class_acc:
                        class_acc[expected[i]] = 0
                        class_total[expected[i]] = 0

                    class_total[expected[i]] += 1
                    if expected[i] == actual[i]:
                        class_acc[expected[i]] += 1

                avg_acc.append(acc.item())
        avg_acc = np.mean(avg_acc)

        for classId in class_acc:
            class_acc[classId] = class_acc[classId] / class_total[classId]

        return avg_acc
