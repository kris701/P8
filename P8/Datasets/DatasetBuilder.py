from .UCR import UCR

def GetDataset(name : str):
    if name == "UCR":
        return UCR
