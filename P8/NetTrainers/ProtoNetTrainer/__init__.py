from .prototypical_batch_sampler import PrototypicalBatchSampler
from .prototypical_loss import prototypical_loss as loss_fn
from .ProtoNetOptions import ProtoNetOptions
from .DataloaderVerifier import DataLoaderVerifier

import torch.utils.data as data
from tqdm import tqdm
import numpy as np
import torch
import os
import torch.nn as nn
import shutil

class ProtoNetTrainer():
    Options : ProtoNetOptions = None;
    Dataset : data.Dataset = None;
    TrainDataloader : data.DataLoader = None;
    ValDataloader : data.DataLoader = None;
    TestDataloader : data.DataLoader = None;

    _isTrained : bool = False;
    BestModel : nn.Module = None;

    def __init__(self, options : ProtoNetOptions, dataset : data.Dataset):
        self.Options = options;
        self.Dataset = dataset;
        if os.path.exists(self.Options.experiment_root):
            shutil.rmtree(self.Options.experiment_root)
            os.makedirs(self.Options.experiment_root)
        else:
            os.makedirs(self.Options.experiment_root)
        if torch.cuda.is_available() and not self.Options.cuda:
            print("WARNING: You have a CUDA device, so you should probably run with --cuda")

        self._init_seed()

        self.ValDataloader = self._init_dataloader('val', self.Dataset)
        self.TrainDataloader = self._init_dataloader('train', self.Dataset)
        self.TestDataloader = self._init_dataloader('test', self.Dataset)

        DataLoaderVerifier.Verify([self.ValDataloader, self.TrainDataloader, self.TestDataloader])


    def _init_seed(self) -> None:
        torch.cuda.cudnn_enabled = False
        np.random.seed(self.Options.manual_seed)
        torch.manual_seed(self.Options.manual_seed)
        torch.cuda.manual_seed(self.Options.manual_seed)

    def _init_dataset(self, mode : str, dataset : data.Dataset) -> data.Dataset:
        return dataset(mode=mode, root=self.Options.dataset_root)

    def _init_sampler(self, labels : list, mode : str) -> PrototypicalBatchSampler:
        if 'train' == mode:
            classes_per_it = self.Options.classes_per_it_tr
            num_samples = self.Options.num_support_tr + self.Options.num_query_tr
        elif 'val' == mode:
            classes_per_it = self.Options.classes_per_it_val
            num_samples = self.Options.num_support_val + self.Options.num_query_val
        else:
            classes_per_it = self.Options.classes_per_it_test
            num_samples = self.Options.num_support_test + self.Options.num_query_test

        return PrototypicalBatchSampler(labels=labels,
                                        classes_per_it=classes_per_it,
                                        num_samples=num_samples,
                                        iterations=self.Options.iterations)

    def _init_dataloader(self, mode : str, dataset : data.Dataset) -> data.DataLoader:
        dataset_ = self._init_dataset(mode, dataset=dataset)
        sampler = self._init_sampler(dataset_.y, mode)
        dataloader = data.DataLoader(dataset_, batch_sampler=sampler)
        return dataloader

    def _init_protonet(self, protonet : nn.Module) -> nn.Module:
        device = 'cuda:0' if torch.cuda.is_available() and self.Options.cuda else 'cpu'
        model = protonet(
            self.Options.x_dim,
            self.Options.hid_dim,
            self.Options.z_dim
            ).to(device)
        return model

    def _init_optim(self, model : nn.Module) -> torch.optim.Optimizer:
        return torch.optim.Adam(params=model.parameters(),
                                lr=self.Options.learning_rate)

    def _init_lr_scheduler(self, optim : torch.optim.Optimizer) -> torch.optim.lr_scheduler._LRScheduler:
        return torch.optim.lr_scheduler.StepLR(optimizer=optim,
                                               gamma=self.Options.lr_scheduler_gamma,
                                               step_size=self.Options.lr_scheduler_step)

    def _save_list_to_file(self, path : str, thelist : list):
        with open(path, 'w') as f:
            for item in thelist:
                f.write("%s\n" % item)


    def _train(self, tr_dataloader : data.DataLoader, model : nn.Module, optim : torch.optim.Optimizer, lr_scheduler : torch.optim.lr_scheduler._LRScheduler, val_dataloader : data.DataLoader = None):
        device = 'cuda:0' if torch.cuda.is_available() and self.Options.cuda else 'cpu'

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
            pbar2 = tqdm(tr_iter, desc='Initializing model...', leave=False, position=1, colour="green")
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
            pbar2 = tqdm(val_iter, desc='Initializing model...', leave=False, position=1, colour="blue")
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

    def _test(self, test_dataloader : data.DataLoader, model : nn.Module) -> float:
        device = 'cuda:0' if torch.cuda.is_available() and self.Options.cuda else 'cpu'
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

    def Train(self) -> float:
        model = self._init_protonet(self.Options.backbone)
        optim = self._init_optim(model)
        lr_scheduler = self._init_lr_scheduler(optim)
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

    def Test(self) -> float:
        if self._isTrained == False:
            raise Exception("Model is not trained! Cannot test on it.")

        return self._test(
                test_dataloader=self.TestDataloader,
                model=self.BestModel)