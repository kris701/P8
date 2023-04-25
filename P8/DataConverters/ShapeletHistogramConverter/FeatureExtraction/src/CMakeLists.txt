set(CMAKE_CXX_STANDARD 20)
add_library(Sources INTERFACE
        IO/FileHanding.h misc/Constants.h core/WindowGeneration.h core/InformationGain.h core/feature_finding/FeatureFinding.h IO/Logger.h
        IO/ArgumentParser.h core/attributes/Attribute.h types/Feature.h core/attributes/Frequency.h core/attributes/MinDist.h core/attributes/AttributeBuilder.h core/feature_finding/Evaluation.h
        core/attributes/MaxDist.h core/attributes/OccPos.h preprocessing/DataAugmentation.h types/FeatureSet.h types/FeatureHistogram.h types/FeatureHistogramBuilder.h types/FeatureHistogramSet.h types/ClassCount.h types/Series.h types/SeriesMap.h types/SeriesSet.h types/ValueLine.h)