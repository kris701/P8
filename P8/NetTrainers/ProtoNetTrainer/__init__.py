# coding=utf-8
from .prototypical_batch_sampler import PrototypicalBatchSampler
from .prototypical_loss import prototypical_loss as loss_fn
from .ProtoNetOptions import ProtoNetOptions

import torch.utils.data as data
from tqdm import tqdm
import numpy as np
import torch
import os
import torch.nn as nn
import shutil

class ProtoNetTrainer():
    _options : ProtoNetOptions = None;

    def __init__(self, options : ProtoNetOptions):
        self._options = options;
        if os.path.exists(self._options.experiment_root):
            shutil.rmtree(self._options.experiment_root)
        else:
            os.makedirs(self._options.experiment_root)
        if torch.cuda.is_available() and not self._options.cuda:
            print("WARNING: You have a CUDA device, so you should probably run with --cuda")

    def _init_seed(self) -> None:
        torch.cuda.cudnn_enabled = False
        np.random.seed(self._options.manual_seed)
        torch.manual_seed(self._options.manual_seed)
        torch.cuda.manual_seed(self._options.manual_seed)


    def _init_dataset(self, mode : str, dataset : data.Dataset) -> data.Dataset:
        dataset_ : data.Dataset = dataset(mode=mode, root=self._options.dataset_root)
        n_classes = len(np.unique(dataset_.y))
        if n_classes < self._options.classes_per_it_tr or n_classes < self._options.classes_per_it_val:
            raise(Exception('There are not enough classes in the dataset in order ' +
                            'to satisfy the chosen classes_per_it. Decrease the ' +
                            'classes_per_it_{tr/val} option and try again.'))
        return dataset_


    def _init_sampler(self, labels : list, mode : str) -> PrototypicalBatchSampler:
        if 'train' in mode:
            classes_per_it = self._options.classes_per_it_tr
            num_samples = self._options.num_support_tr + self._options.num_query_tr
        else:
            classes_per_it = self._options.classes_per_it_val
            num_samples = self._options.num_support_val + self._options.num_query_val

        return PrototypicalBatchSampler(labels=labels,
                                        classes_per_it=classes_per_it,
                                        num_samples=num_samples,
                                        iterations=self._options.iterations)


    def _init_dataloader(self, mode : str, dataset : data.Dataset) -> data.DataLoader:
        dataset_ = self._init_dataset(mode, dataset=dataset)
        sampler = self._init_sampler(dataset_.y, mode)
        dataloader = data.DataLoader(dataset_, batch_sampler=sampler)
        return dataloader


    def _init_protonet(self, protonet : nn.Module) -> nn.Module:
        device = 'cuda:0' if torch.cuda.is_available() and self._options.cuda else 'cpu'
        model = protonet(
            self._options.x_dim,
            self._options.hid_dim,
            self._options.z_dim
            ).to(device)
        return model

    def _init_optim(self, model : nn.Module) -> torch.optim.Optimizer:
        return torch.optim.Adam(params=model.parameters(),
                                lr=self._options.learning_rate)

    def _init_lr_scheduler(self, optim : torch.optim.Optimizer) -> torch.optim.lr_scheduler._LRScheduler:
        return torch.optim.lr_scheduler.StepLR(optimizer=optim,
                                               gamma=self._options.lr_scheduler_gamma,
                                               step_size=self._options.lr_scheduler_step)

    def _save_list_to_file(self, path : str, thelist : list):
        with open(path, 'w') as f:
            for item in thelist:
                f.write("%s\n" % item)


    def _train(self, tr_dataloader : data.DataLoader, model : nn.Module, optim : torch.optim.Optimizer, lr_scheduler : torch.optim.lr_scheduler._LRScheduler, val_dataloader : data.DataLoader = None):
        device = 'cuda:0' if torch.cuda.is_available() and self._options.cuda else 'cpu'

        if val_dataloader is None:
            best_state = None
        train_loss = []
        train_acc = []
        val_loss = []
        val_acc = []
        best_acc = 0

        best_model_path = os.path.join(self._options.experiment_root, 'best_model.pth')
        last_model_path = os.path.join(self._options.experiment_root, 'last_model.pth')

        pbar1 = tqdm(range(self._options.train_epochs), desc='Loading...', position=0, colour="red")
        for epoch in pbar1:
            pbar1.set_description('Epoch {}, (Best Acc: {:0.2f})'.format(epoch + 1, best_acc), refresh=True);
            tr_iter = iter(tr_dataloader)
            model.train()
            pbar2 = tqdm(tr_iter, desc='Initializing model...', leave=False, position=1, colour="green")
            for batch in pbar2:
                optim.zero_grad()
                x, y = batch
                x, y = x.to(device), y.to(device)
                model_output = model(x)
                loss, acc = loss_fn(model_output, target=y, n_support=self._options.num_support_tr)
                loss.backward()
                optim.step()
                train_loss.append(loss.item())
                train_acc.append(acc.item())
                avg_loss = np.mean(train_loss[-self._options.iterations:])
                avg_acc = np.mean(train_acc[-self._options.iterations:])
                pbar2.set_description('   Train Loss: {:0.2f}, Train Acc: {:0.2f}'.format(avg_loss, avg_acc), refresh=True);
            lr_scheduler.step()
            if val_dataloader is None:
                continue
            val_iter = iter(val_dataloader)
            model.eval()
            pbar2 = tqdm(val_iter, desc='Initializing model...', leave=False, position=1, colour="blue")
            for batch in pbar2:
                x, y = batch
                x, y = x.to(device), y.to(device)
                model_output = model(x)
                loss, acc = loss_fn(model_output, target=y,
                                    n_support=self._options.num_support_val)
                val_loss.append(loss.item())
                val_acc.append(acc.item())
                avg_loss = np.mean(val_loss[-self._options.iterations:])
                avg_acc = np.mean(val_acc[-self._options.iterations:])
                pbar2.set_description('   Val Loss: {:0.2f}, Val Acc: {:0.2f}    '.format(avg_loss, avg_acc), refresh=True);
            if avg_acc >= best_acc:
                torch.save(model.state_dict(), best_model_path)
                best_acc = avg_acc
                best_state = model.state_dict()

        torch.save(model.state_dict(), last_model_path)

        print("Outputting best model to '{}'".format(self._options.experiment_root))
        for name in ['train_loss', 'train_acc', 'val_loss', 'val_acc']:
            self._save_list_to_file(os.path.join(self._options.experiment_root,
                                           name + '.txt'), locals()[name])

        return best_state, best_acc, train_loss, train_acc, val_loss, val_acc


    def _test(self, test_dataloader : data.DataLoader, model : nn.Module):
        device = 'cuda:0' if torch.cuda.is_available() and self._options.cuda else 'cpu'
        avg_acc = list()
        pbar1 = tqdm(range(self._options.test_epochs), desc='Loading...', position=0, colour="red")
        for epoch in pbar1:
            pbar1.set_description('Epoch {}'.format(epoch + 1), refresh=True);
            test_iter = iter(test_dataloader)
            for batch in test_iter:
                x, y = batch
                x, y = x.to(device), y.to(device)
                model_output = model(x)
                _, acc = loss_fn(model_output, target=y,
                                 n_support=self._options.num_support_val)
                avg_acc.append(acc.item())
        avg_acc = np.mean(avg_acc)
        print('Avr Test Acc: {}'.format(avg_acc))

        return avg_acc


    def _eval(self):
        self._init_seed()
        test_dataloader = self._init_dataset(self._options)[-1]
        model = self._init_protonet()
        model_path = os.path.join(self._options.experiment_root, 'best_model.pth')
        model.load_state_dict(torch.load(model_path))

        self._test(
             test_dataloader=test_dataloader,
             model=model)


    def Train(self, dataset : data.Dataset) -> nn.Module:
        self._init_seed()

        tr_dataloader = self._init_dataloader('train', dataset)
        trainval_dataloader = self._init_dataloader('trainval', dataset)

        model = self._init_protonet(self._options.backbone)
        optim = self._init_optim(model)
        lr_scheduler = self._init_lr_scheduler(optim)
        res = self._train(
                    tr_dataloader=tr_dataloader,
                    val_dataloader=trainval_dataloader,
                    model=model,
                    optim=optim,
                    lr_scheduler=lr_scheduler)
        best_state, best_acc, train_loss, train_acc, val_loss, val_acc = res
        
        test_dataloader = self._init_dataloader('test', dataset)
        model.load_state_dict(best_state)
        print('Testing with best model..')
        self._test(
                test_dataloader=test_dataloader,
                model=model)

        return model
