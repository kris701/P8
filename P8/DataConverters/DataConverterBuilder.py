from .DirectDataConverter import DirectDataConverter
from .ShapeletHistogramConverter import ShapeletHistogramConverter

def GetDataConverter(name : str):
    if name == "DirectDataConverter":
        return DirectDataConverter
    elif name == "ShapeletHistogramConverter":
        return ShapeletHistogramConverter

