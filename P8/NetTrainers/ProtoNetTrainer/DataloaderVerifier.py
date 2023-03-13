import torch.utils.data as data

class DataLoaderVerifier():
    def Verify(loaders : list):
        dictChecker = {}
        for loader in loaders:
            for item in loader.dataset.all_items:
                if item in dictChecker:
                    raise Exception("Error! Datasets are overlapping!")
                dictChecker[item] = 1;



