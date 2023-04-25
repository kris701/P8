#ifndef FEATUREEXTRACTION_SAMPLING_H
#define FEATUREEXTRACTION_SAMPLING_H

#include <optional>
#include "types/SeriesMap.h"

namespace Sampling {
    struct Sampler {
        virtual std::optional<SeriesMap> Sample(const SeriesMap &data) = 0;
    };

    struct RandomSampler : public Sampler {
        static inline uint MAX_ATTEMPTS = 10;
        std::optional<SeriesMap> Sample(const SeriesMap &data) final {
            uint attempts = 0;
            std::optional<SeriesMap> sample;

            SeriesMap tempSample;
            uint diffClassCount;
            do {
                if (attempts++ > MAX_ATTEMPTS)
                    return sample;

                diffClassCount = 0;
                for (const auto &seriesSet : data) {
                    const uint sampleSize = rand() % seriesSet.second.size();
                    std::sample(seriesSet.second.begin(), seriesSet.second.end(), std::back_inserter(tempSample[seriesSet.first]), sampleSize, rd);
                }
            } while (
                    std::find(priorSamples.begin(), priorSamples.end(), sample) != priorSamples.end() &&
                    tempSample.size() < 2);

            return tempSample;
        }
    private:
        std::vector<SeriesMap> priorSamples;
    };
}

#endif //FEATUREEXTRACTION_SAMPLING_H
