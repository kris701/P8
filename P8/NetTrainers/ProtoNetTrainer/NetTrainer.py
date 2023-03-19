from .prototypical_loss import prototypical_loss as loss_fn
from .BaseNetTrainer import BaseNetTrainer
from .NetOptions import NetOptions

import torch.utils.data as data
from tqdm import tqdm
import numpy as np
import torch
import os
import torch.nn as nn

class NetTrainer(BaseNetTrainer):
    def __init__(self, options: NetOptions, dataset: data.Dataset):
        super().__init__(options, dataset)

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

        pbar1 = tqdm(range(self.Options.train_epochs), desc='Loading...', position=0, colour="red")
        for epoch in pbar1:
            pbar1.set_description('Epoch {}, (Best Acc: {:0.2f})'.format(epoch + 1, best_acc), refresh=True);
            tr_iter = iter(tr_dataloader)

            # Train
            model.train()
            pbar2 = tqdm(tr_iter, desc='   Initializing model...', leave=False, position=1, colour="green")
            for batch in pbar2:
                optim.zero_grad()
                x, y = batch
                x, y = x.to(device), y.to(device)
                model_output = model(x)
                loss, acc = loss_fn(model_output, target=y, n_support=self.Options.num_support_tr)
                loss.backward()
                optim.step()
                train_loss.append(loss.item())
                train_acc.append(acc.item())
                avg_loss = np.mean(train_loss[-self.Options.iterations:])
                avg_acc = np.mean(train_acc[-self.Options.iterations:])
                pbar2.set_description('   Train Loss: {:0.2f}, Train Acc: {:0.2f}'.format(avg_loss, avg_acc), refresh=True);
            lr_scheduler.step()
            if val_dataloader is None:
                continue
            val_iter = iter(val_dataloader)

            # Eval
            model.eval()
            pbar2 = tqdm(val_iter, desc='   Initializing model...', leave=False, position=1, colour="blue")
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

        print("Outputting best model to '{}'".format(self.Options.experiment_root))
        for name in ['train_loss', 'train_acc', 'val_loss', 'val_acc']:
            self._save_list_to_file(os.path.join(self.Options.experiment_root,
                                           name + '.txt'), locals()[name])

        return best_state, best_acc, train_loss, train_acc, val_loss, val_acc

    def Test(self) -> float:
        '''
        Tests the best model with a set of new data.
        '''
        if self._isTrained == False:
            raise Exception("Model is not trained! Cannot test on it.")

        return self._test(
                test_dataloader=self.TestDataloader,
                model=self.BestModel)

    def _test(self, test_dataloader : data.DataLoader, model : nn.Module) -> float:
        '''
        Tests a model with a set of new data.
        '''
        device = self._getDevice()
        avg_acc = list()
        pbar1 = tqdm(range(self.Options.test_epochs), desc='Loading...', position=0, colour="red")
        for epoch in pbar1:
            pbar1.set_description('Epoch {}'.format(epoch + 1), refresh=True);
            test_iter = iter(test_dataloader)
            for batch in test_iter:
                x, y = batch
                x, y = x.to(device), y.to(device)
                model_output = model(x)
                _, acc = loss_fn(model_output, target=y,
                                 n_support=self.Options.num_support_test)
                avg_acc.append(acc.item())
        avg_acc = np.mean(avg_acc)

        return avg_acc
