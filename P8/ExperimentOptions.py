class ExperimentOptions():
    ExperimentConfigDir : str = "Experiments/Configs";
    ExperimentResultsDir : str = "Experiments/Results";
    ExperimentsToRun : list;
    ExperimentName : str = "Ours"
    BaseConfig : str = "Experiments/Configs/Base.ini"

    GenerateGraphs : bool = False
    GenerateClassGraphs : bool = False
    GenerateShapeletGraphs : bool = False

    ComparisonData : list = []

    DebugMode : bool = True;

    ZipDataset : bool = True;
