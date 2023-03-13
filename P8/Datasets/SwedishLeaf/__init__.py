# coding=utf-8
from __future__ import print_function
import torch.utils.data as data
from PIL import Image
import numpy as np
import shutil
import errno
import torch
import os

'''
Inspired by https://github.com/pytorch/vision/pull/46
'''


class SwedishLeafDataset(data.Dataset):

    def __init__(self, mode='train', root='..' + os.sep + 'dataset' + os.sep + 'swedishleaf', transform=None, target_transform=None):
        '''
        The items are (filename,category). The index of all the categories can be found in self.idx_classes
        Args:
        - root: the directory where the dataset will be stored
        - transform: how to transform the input
        - target_transform: how to transform the target
        '''
        super(SwedishLeafDataset, self).__init__()
        self.mode = mode
        self.root = root
        self.splits_folder = "split"
        self.processed_folder = "data"
        self.transform = transform
        self.target_transform = target_transform

        self.classes = get_current_classes(os.path.join(self.root, self.splits_folder, mode + '.txt'))
        item_file = open(os.path.join(root, self.splits_folder, mode) + '.txt', 'r')
        self.all_items = item_file.read().split('\n')
        item_file.close()

        self.idx_classes = self.index_classes(self.all_items)

        self.x = map(self.load_item, self.all_items)
        self.x = list(self.x)
        self.x = torch.tensor(self.x)

        self.y = list(map(find_y, self.all_items))
        print(mode, len(self.x), len(self.y))

    def __getitem__(self, idx):
        x = self.x[idx]
        if self.transform:
            x = self.transform(x)
        return x, self.y[idx]

    def __len__(self):
        return len(self.all_items)

    def _check_exists(self):
        return os.path.exists(os.path.join(self.root, self.processed_folder))
    
    def index_classes(self, items):
        if self.mode == 'train':
            classes = {1: 0, 2: 1, 3: 2, 4: 3, 10: 4, 11: 5, 12: 6, 13: 7, 14: 8, 15: 9}
        else:
            classes = {5: 0, 6: 1, 7: 2, 8: 3, 9: 4}
        return classes
    
    def load_item(self, item):
        item_path = os.path.join(self.root, item)
        f = open(item_path, 'r')
        result = f.read().split('\n')
        result = result[:-1] #to get rid of the final empty line
        f.close()
        result = list(map(float, result))
        return result
    
def find_y(item):
        return int(item.split(os.sep)[1])

def get_current_classes(fname):
    with open(fname) as f:
        paths = f.read().split("\n")
    classes = [path.split(os.sep)[1] for path in paths]
    return classes
