from .ProtoNetTrainer.NetTrainer import NetTrainer as nt1
from .NaiveKNN.NetTrainer import NetTrainer as nt2

def GetNetTrainer(name : str):
    if name == "ProtoNetTrainer":
        return nt1
    elif name == "NaiveKNN":
        return nt2

