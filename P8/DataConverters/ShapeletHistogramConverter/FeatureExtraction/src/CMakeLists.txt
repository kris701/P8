set(CMAKE_CXX_STANDARD 20)
add_library(Sources INTERFACE
        IO/FileHanding.hpp misc/Constants.hpp core/WindowGeneration.hpp core/InformationGain.hpp core/feature_finding/FeatureFinding.hpp IO/Logger.hpp
        IO/ArgumentParser.hpp core/attributes/Attribute.hpp types/Feature.hpp core/attributes/Frequency.hpp core/attributes/MinDist.hpp core/attributes/AttributeBuilder.hpp core/feature_finding/Evaluation.hpp
        core/attributes/MaxDist.hpp core/attributes/OccPos.hpp preprocessing/DataAugmentation.hpp types/FeatureSet.hpp types/FeatureHistogram.hpp types/FeatureHistogramBuilder.hpp types/FeatureHistogramSet.hpp types/ClassCount.hpp types/Series.hpp types/SeriesMap.hpp types/SeriesSet.hpp types/ValueLine.hpp)
