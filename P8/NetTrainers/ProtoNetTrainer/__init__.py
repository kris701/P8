# coding=utf-8
from .prototypical_batch_sampler import PrototypicalBatchSampler
from .prototypical_loss import prototypical_loss as loss_fn
from .ProtoNetOptions import ProtoNetOptions

import torch.utils.data as data
from tqdm import tqdm
import numpy as np
import torch
import os

class ProtoNetTrainer():
    _options : ProtoNetOptions = None;

    def __init__(self, options : ProtoNetOptions):
        self._options = options;
        if not os.path.exists(self._options.experiment_root):
            os.makedirs(self._options.experiment_root)
        if torch.cuda.is_available() and not self._options.cuda:
            print("WARNING: You have a CUDA device, so you should probably run with --cuda")

    def forward(self, x):
        x = x.unsqueeze(1)  # Add a channel dimension
        x = self.encoder(x)
        return x.view(x.size(0), -1)

    def init_seed(self, opt):
        '''
        Disable cudnn to maximize reproducibility
        '''
        torch.cuda.cudnn_enabled = False
        np.random.seed(opt.manual_seed)
        torch.manual_seed(opt.manual_seed)
        torch.cuda.manual_seed(opt.manual_seed)


    def init_dataset(self, opt, mode, dataset):
        dataset_ = dataset(mode=mode, root=opt.dataset_root)
        n_classes = len(np.unique(dataset_.y))
        if n_classes < opt.classes_per_it_tr or n_classes < opt.classes_per_it_val:
            raise(Exception('There are not enough classes in the dataset in order ' +
                            'to satisfy the chosen classes_per_it. Decrease the ' +
                            'classes_per_it_{tr/val} option and try again.'))
        return dataset_


    def init_sampler(self, opt, labels, mode):
        if 'train' in mode:
            classes_per_it = opt.classes_per_it_tr
            num_samples = opt.num_support_tr + opt.num_query_tr
        else:
            classes_per_it = opt.classes_per_it_val
            num_samples = opt.num_support_val + opt.num_query_val

        return PrototypicalBatchSampler(labels=labels,
                                        classes_per_it=classes_per_it,
                                        num_samples=num_samples,
                                        iterations=opt.iterations)


    def init_dataloader(self, opt, mode, dataset):
        dataset_ = self.init_dataset(opt, mode, dataset=dataset)
        sampler = self.init_sampler(opt, dataset_.y, mode)
        dataloader = torch.utils.data.DataLoader(dataset_, batch_sampler=sampler)
        return dataloader


    def init_protonet(self, opt, protonet):
        '''
        Initialize the ProtoNet
        '''
        device = 'cuda:0' if torch.cuda.is_available() and opt.cuda else 'cpu'
        model = protonet(
            self._options.x_dim,
            self._options.hid_dim,
            self._options.z_dim
            ).to(device)
        return model


    def init_optim(self, opt, model):
        '''
        Initialize optimizer
        '''
        return torch.optim.Adam(params=model.parameters(),
                                lr=opt.learning_rate)


    def init_lr_scheduler(self, opt, optim):
        '''
        Initialize the learning rate scheduler
        '''
        return torch.optim.lr_scheduler.StepLR(optimizer=optim,
                                               gamma=opt.lr_scheduler_gamma,
                                               step_size=opt.lr_scheduler_step)


    def save_list_to_file(self, path, thelist):
        with open(path, 'w') as f:
            for item in thelist:
                f.write("%s\n" % item)


    def train(self, opt, tr_dataloader, model, optim, lr_scheduler, val_dataloader=None):
        '''
        Train the model with the prototypical learning algorithm
        '''

        device = 'cuda:0' if torch.cuda.is_available() and opt.cuda else 'cpu'

        if val_dataloader is None:
            best_state = None
        train_loss = []
        train_acc = []
        val_loss = []
        val_acc = []
        best_acc = 0

        best_model_path = os.path.join(opt.experiment_root, 'best_model.pth')
        last_model_path = os.path.join(opt.experiment_root, 'last_model.pth')

        for epoch in range(opt.epochs):
            print('=== Epoch: {} ==='.format(epoch))
            tr_iter = iter(tr_dataloader)
            model.train()
            for batch in tqdm(tr_iter):
                optim.zero_grad()
                x, y = batch
                x, y = x.to(device), y.to(device)
                model_output = model(x)
                loss, acc = loss_fn(model_output, target=y,
                                    n_support=opt.num_support_tr)
                loss.backward()
                optim.step()
                train_loss.append(loss.item())
                train_acc.append(acc.item())
            avg_loss = np.mean(train_loss[-opt.iterations:])
            avg_acc = np.mean(train_acc[-opt.iterations:])
            print('Avg Train Loss: {}, Avg Train Acc: {}'.format(avg_loss, avg_acc))
            lr_scheduler.step()
            if val_dataloader is None:
                continue
            val_iter = iter(val_dataloader)
            model.eval()
            for batch in val_iter:
                x, y = batch
                x, y = x.to(device), y.to(device)
                model_output = model(x)
                loss, acc = loss_fn(model_output, target=y,
                                    n_support=opt.num_support_val)
                val_loss.append(loss.item())
                val_acc.append(acc.item())
            avg_loss = np.mean(val_loss[-opt.iterations:])
            avg_acc = np.mean(val_acc[-opt.iterations:])
            postfix = ' (Best)' if avg_acc >= best_acc else ' (Best: {})'.format(
                best_acc)
            print('Avg Val Loss: {}, Avg Val Acc: {}{}'.format(
                avg_loss, avg_acc, postfix))
            if avg_acc >= best_acc:
                torch.save(model.state_dict(), best_model_path)
                best_acc = avg_acc
                best_state = model.state_dict()

        torch.save(model.state_dict(), last_model_path)

        for name in ['train_loss', 'train_acc', 'val_loss', 'val_acc']:
            self.save_list_to_file(os.path.join(opt.experiment_root,
                                           name + '.txt'), locals()[name])

        return best_state, best_acc, train_loss, train_acc, val_loss, val_acc


    def test(self, opt, test_dataloader, model):
        '''
        Test the model trained with the prototypical learning algorithm
        '''
        device = 'cuda:0' if torch.cuda.is_available() and opt.cuda else 'cpu'
        avg_acc = list()
        for epoch in range(10):
            test_iter = iter(test_dataloader)
            for batch in test_iter:
                x, y = batch
                x, y = x.to(device), y.to(device)
                model_output = model(x)
                _, acc = loss_fn(model_output, target=y,
                                 n_support=opt.num_support_val)
                avg_acc.append(acc.item())
        avg_acc = np.mean(avg_acc)
        print('Test Acc: {}'.format(avg_acc))

        return avg_acc


    def eval(self, opt):
        '''
        Initialize everything and train
        '''
        options = self._options;

        if torch.cuda.is_available() and not options.cuda:
            print("WARNING: You have a CUDA device, so you should probably run with --cuda")

        self.init_seed(options)
        test_dataloader = self.init_dataset(options)[-1]
        model = self.init_protonet(options)
        model_path = os.path.join(opt.experiment_root, 'best_model.pth')
        model.load_state_dict(torch.load(model_path))

        self.test(opt=options,
             test_dataloader=test_dataloader,
             model=model)


    def Run(self, dataset : data.Dataset):
        '''
        Initialize everything and train
        '''
        self.init_seed(self._options)

        tr_dataloader = self.init_dataloader(self._options, 'train', dataset)
        #val_dataloader = init_dataloader(options, 'val', dataset) #
        trainval_dataloader = self.init_dataloader(self._options, 'trainval', dataset)
        test_dataloader = self.init_dataloader(self._options, 'test', dataset)

        model = self.init_protonet(self._options, self._options.backbone)
        optim = self.init_optim(self._options, model)
        lr_scheduler = self.init_lr_scheduler(self._options, optim)
        res = self.train(opt=self._options,
                    tr_dataloader=tr_dataloader,
                    val_dataloader=trainval_dataloader, #or it will bug out. For some reason, simply using the val dataloader causes a bug. Presumably there are too few samples in the validation data set
                    model=model,
                    optim=optim,
                    lr_scheduler=lr_scheduler)
        best_state, best_acc, train_loss, train_acc, val_loss, val_acc = res
        if self._options.do_train == True:
            print('Testing with last model..')
            self.test(opt=self._options,
                 test_dataloader=test_dataloader,
                 model=model)

        if self._options.do_test == True:
            model.load_state_dict(best_state)
            print('Testing with best model..')
            self.test(opt=self._options,
                 test_dataloader=test_dataloader,
                 model=model)

        # optim = init_optim(options, model)
        # lr_scheduler = init_lr_scheduler(options, optim)

        # print('Training on train+val set..')
        # train(opt=options,
        #       tr_dataloader=trainval_dataloader,
        #       val_dataloader=None,
        #       model=model,
        #       optim=optim,
        #       lr_scheduler=lr_scheduler)

        # print('Testing final model..')
        # test(opt=options,
        #      test_dataloader=test_dataloader,
        #      model=model)
