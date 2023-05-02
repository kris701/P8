#ifndef FEATUREEXTRACTION_ATTRIBUTEBUILDER_H
#define FEATUREEXTRACTION_ATTRIBUTEBUILDER_H

#include <vector>
#include <memory>
#include <sstream>
#include "Attribute.hpp"
#include "MinDist.hpp"
#include "MaxDist.hpp"
#include "Frequency.hpp"
#include "OccPos.hpp"
#include <clocale>

namespace AttributeBuilder {
    static std::shared_ptr<Attribute> GenerateFrequency(const std::string &att) {
        uint i = 0;
        while (i < att.size() && std::isalpha(att[i++])){}
        i--;
        const double tolerance = std::atof(att.substr(i, att.size() - i).c_str());
        return std::make_shared<Frequency>(Frequency(tolerance));
    }

    static std::shared_ptr<Attribute> GenerateOccPos(const std::string &att) {
        uint i = 0;
        while (i < att.size() && std::isalpha(att[i++])){}
        i--;
        const double tolerance = std::atof(att.substr(i, att.size() - i).c_str());
        return std::make_shared<OccPos>(OccPos(tolerance));
    }

    static std::shared_ptr<Attribute> GenerateMinDist() {
        return std::make_shared<MinDist>(MinDist());
    }

    static std::shared_ptr<Attribute> GenerateMaxDist() {
        return std::make_shared<MaxDist>(MaxDist());
    }

    static std::vector<std::shared_ptr<Attribute>> GenerateAttributes(const std::vector<std::string> &stringAttributes) {
        std::vector<std::shared_ptr<Attribute>> attributes;

        for (const auto &att : stringAttributes) {
            std::string name;
            for (char c : att)
                if (std::isalpha(c))
                    name += c;
                else
                    break;
            std::transform(name.begin(), name.end(), name.begin(),
                           [](unsigned char c){ return std::tolower(c); });

            if (name == "freq")
                attributes.push_back(GenerateFrequency(att));
            else if (name == "occpos")
                attributes.push_back(GenerateOccPos(att));
            else if (name == "mindist")
                attributes.push_back(GenerateMinDist());
            else if (name == "maxdist")
                attributes.push_back(GenerateMaxDist());
            else
                throw std::invalid_argument("Unknown attribute");
        }

        return attributes;
    }
}

#endif //FEATUREEXTRACTION_ATTRIBUTEBUILDER_H
