class ExperimentOptions():
    ExperimentConfigDir : str = "Experiments/Configs";
    ExperimentResultsDir : str = "Experiments/Results";
    ExperimentsToRun : list;
    ExperimentName : str = "Ours"
    BaseConfig : str = "Experiments/Configs/Base.ini"

    GenerateGraphs : bool = False
    GenerateExperimentGraph : bool = False
    GenerateClassGraphs : bool = False
    GenerateShapeletGraphs : bool = False
    GenerateSourceGraphs : bool = False

    ComparisonData : list = []

    DebugMode : bool = True;

    ZipDataset : bool = True;
    CopyConfigs : bool = True;

    ForceRemakeDataset : bool = False;