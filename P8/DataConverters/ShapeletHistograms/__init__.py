import cppyy
from ..DataConverterOptions import DataConverterOptions
from ..BaseDataConverter import BaseDataConverter

class ShapeletFeatureExtractionWrapper(BaseDataConverter):
    def __init__(self, options: DataConverterOptions) -> None:
        super().__init__(options)

    def _Initialize(self):
        # Include whatever cpp files are needed
        cppyy.include('FeatureExtraction/src/SeriesUtils.h')
        cppyy.include('FeatureExtraction/src/Types.h')

    def ConvertData(self):
        s = cppyy.gbl.abc()
        b = cppyy.gbl.LabelledSeries(1, [1,2,3,4])
        print(s.MinValue([b]))