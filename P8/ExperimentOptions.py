class ExperimentOptions():
    BaseSuiteConfig : str = "";

    FormatDataset : bool = True;
    RunTrain : bool = True;
    RunTest : bool = True;

    ExperimentConfigDir : str = "Experiments/Configs";
    ExperimentResultsDir : str = "Experiments/Results";
    ExperimentsToRun : list;
    ExperimentRounds : int = 1;
    ExperimentName : str = "Ours"
    BaseConfig : str = "Experiments/Configs/Base.ini"

    GenerateGraphs : bool = False
    GenerateAccuracyGraphs : bool = False
    GenerateClassAccuracyGraph : bool = False
    GenerateExperimentGraph : bool = False
    GenerateClassGraphs : bool = False
    GenerateShapeletGraphs : bool = False
    GenerateOriginalSourceGraph : bool = False
    GenerateAugmentedSourceGraph : bool = False
    GenerateCandidatesGraph : bool = False
    GenerateRejectsGraph : bool = False

    ComparisonData : list = []

    DebugMode : bool = True;

    ZipDataset : bool = True;
    CopyConfigs : bool = True;

    ForceRemakeDataset : bool = False;

    MaxProcessesToSpawn : int = 8;