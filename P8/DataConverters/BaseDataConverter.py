from .DataConverterOptions import DataConverterOptions

class BaseDataConverter(object):
    Options : DataConverterOptions = None;
    
    def __init__(self, options : DataConverterOptions) -> None:
        self.Options = options

    def ConvertData(self):
        raise Exception("Not Implemented!")




