from __future__ import print_function
import torch.utils.data as data
import torch
import os

'''
Inspired by https://github.com/pytorch/vision/pull/46
'''

class SwedishLeafDataset(data.Dataset):

    def __init__(self, mode='train', root='formated', transform=None, target_transform=None):
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

        self.classes = self._get_current_classes(os.path.join(self.root, self.splits_folder, mode + '.txt'))
        item_file = open(os.path.join(root, self.splits_folder, mode) + '.txt', 'r')
        self.all_items = item_file.read().split('\n')
        item_file.close()

        self.idx_classes = self._index_classes(self.all_items)

        self.x = map(self._load_item, self.all_items)
        self.x = list(self.x)
        self.x = torch.tensor(self.x)

        self.y = list(map(self._find_y, self.all_items))
        #print(mode, len(self.x), len(self.y))

    def __getitem__(self, idx):
        x = self.x[idx]
        if self.transform:
            x = self.transform(x)
        return x, self.y[idx]

    def __len__(self):
        return len(self.all_items)

    def _check_exists(self):
        return os.path.exists(os.path.join(self.root, self.processed_folder))
    
    def _index_classes(self, items):
        idx = {}
        for i in items:
            index = i[i.find("data\\")+len("data\\"):i.rfind("\\")]
            if not index in idx:
                idx[index] = 1
            else:
                idx[index] = idx[index] + 1
        return idx
    
    def _load_item(self, item):
        item_path = os.path.join(self.root, item)
        f = open(item_path, 'r')
        result = f.read().split('\n')
        result = result[:-1] #to get rid of the final empty line
        f.close()
        result = list(map(float, result))
        return result

    def _get_current_classes(self, fname):
        with open(fname) as f:
            paths = f.read().split("\n")
        classes = [path.split(os.sep)[1] for path in paths]
        return classes

    def _find_y(self, item):
            return int(item.split(os.sep)[1])
