import torch.nn as nn


def conv_block(in_channels, out_channels):
    '''
    returns a block conv-bn-relu-pool
    '''
    return nn.Sequential(
        nn.Conv1d(in_channels, out_channels, kernel_size=2, stride=1, padding=1),
        nn.BatchNorm1d(out_channels),
        nn.ReLU(),
        nn.MaxPool1d(2)
    )


class slProtoNet(nn.Module):
    '''
    Model as described in the reference paper,
    source: https://github.com/jakesnell/prototypical-networks/blob/f0c48808e496989d01db59f86d4449d7aee9ab0c/protonets/models/few_shot.py#L62-L84
    '''
    def __init__(self, x_dim=1, hid_dim=8, z_dim=8):
        super(slProtoNet, self).__init__()
        
        self.encoder = nn.Sequential(
            conv_block(x_dim, hid_dim),
            conv_block(hid_dim, hid_dim),
            conv_block(hid_dim, hid_dim),
            conv_block(hid_dim, z_dim),
        )

    def forward(self, x):
        
        x = x.unsqueeze(1)  # Add a channel dimension
        x = self.encoder(x)
        return x.view(x.size(0), -1)
