#ifndef FEATUREEXTRACTION_SAMPLING_H
#define FEATUREEXTRACTION_SAMPLING_H

#include <optional>
#include <random>
#include "types/SeriesMap.h"

namespace Sampling {
    struct Sampler {
        virtual std::optional<SeriesMap> Sample(const SeriesMap &data) = 0;

    protected:
        static SeriesMap Sample(const SeriesMap &data, ClassCount count) {
            SeriesMap sample;

            for (uint i = 0; i < count.size(); i++)
                if (count.at(i) != 0)
                    std::sample(data.at(i).begin(), data.at(i).end(),
                                std::back_inserter(sample[i]), count.at(i), rd);

            return sample;
        }
    };

    struct RandomSampler : public Sampler {
        static inline uint MAX_ATTEMPTS = 100;
        std::optional<SeriesMap> Sample(const SeriesMap &data) final {
            uint attempts = 0;

            SeriesMap sample;
            do {
                if (attempts++ > MAX_ATTEMPTS)
                    return {};

                sample.clear();
                ClassCount count{0};
                for (const auto &seriesSet : data)
                    count.at(seriesSet.first) = rand() % seriesSet.second.size();

                sample = Sampler::Sample(data, count);

            } while (
                    std::find(priorSamples.begin(), priorSamples.end(), sample) != priorSamples.end() ||
                    sample.size() < 2);

            priorSamples.push_back(sample);
            return sample;
        }
    private:
        std::vector<SeriesMap> priorSamples;
    };

    struct SemiRandomSampler : public Sampler {
        static inline uint MAX_ATTEMPTS = 10;
        std::optional<SeriesMap> Sample(const SeriesMap &data) final {
            uint attempts = 0;

            SeriesMap sample;
            do {
                if (attempts++ > MAX_ATTEMPTS)
                    return {};

                sample.clear();
                ClassCount count{0};
                for (const auto &seriesSet : data) {
                    const auto num = rand() % 100;
                    const uint zeroOdds = (data.size() == 2) ? 0 : 25;
                    if (num < zeroOdds) {
                        count.at(seriesSet.first) = 0;
                        continue;
                    }

                    const uint remainder = 100 - zeroOdds;
                    const uint size = seriesSet.second.size();
                    const uint halfSize = (data.size() == 2) ? 0 : size / 2;
                    const uint sizeOdds = (uint) std::round((double)remainder / (double)halfSize);

                    for (uint i = 0; i <= remainder; i++) {
                        if (zeroOdds + (i * sizeOdds) > num) {
                            count.at(seriesSet.first) = halfSize + i;
                            break;
                        }
                    }
                }

                sample = Sampler::Sample(data, count);

            } while (
                    std::find(priorSamples.begin(), priorSamples.end(), sample) != priorSamples.end() ||
                    sample.size() < 2);

            priorSamples.push_back(sample);
            return sample;
        }
    private:
        std::vector<SeriesMap> priorSamples;
    };
}

#endif //FEATUREEXTRACTION_SAMPLING_H
