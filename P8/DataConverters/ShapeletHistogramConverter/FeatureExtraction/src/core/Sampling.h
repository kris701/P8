#ifndef FEATUREEXTRACTION_SAMPLING_H
#define FEATUREEXTRACTION_SAMPLING_H

#include <optional>
#include <random>
#include "types/SeriesMap.h"
#include "misc/Constants.h"

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
        static inline uint MAX_ATTEMPTS = 100;
        std::optional<SeriesMap> Sample(const SeriesMap &data) final {
            uint attempts = 0;

            SeriesMap sample;
            do {
                if (attempts++ > MAX_ATTEMPTS)
                    return {};

                sample.clear();
                ClassCount count{0};

                for (const auto &seriesSet : data) {
                    std::binomial_distribution<> d(seriesSet.second.size(), 0.5);
                    std::map<uint, uint> hist;
                    for (uint i = 0; i < 100; i++)
                        ++hist[d(g)];

                    const auto r = rand() % 100;
                    uint acc = 0;
                    for (const auto h : hist) {
                        if (acc >= r) {
                            count[seriesSet.first] = h.first;
                            break;
                        }
                        acc += h.second;
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
