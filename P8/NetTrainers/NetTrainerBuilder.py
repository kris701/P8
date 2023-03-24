from .ProtoNetTrainer.NetTrainer import NetTrainer

def GetNetTrainer(name : str):
    if name == "ProtoNetTrainer":
        return NetTrainer

