#ifndef FEATUREEXTRACTION_ATTRIBUTEBUILDER_H
#define FEATUREEXTRACTION_ATTRIBUTEBUILDER_H

#include <vector>
#include <memory>
#include <sstream>
#include "Attribute.h"
#include "MinDist.h"
#include "Frequency.h"

namespace AttributeBuilder {
    static std::shared_ptr<Attribute> GenerateFrequency(const std::string &att) {
        uint i = 0;
        while (i < att.size() && std::isalpha(att[i++])){}
        i--;
        const double tolerance = std::atof(att.substr(i, att.size() - i).c_str());
        return std::make_shared<Frequency>(Frequency(tolerance));
    }

    static std::shared_ptr<Attribute> GenerateMinDist() {
        return std::make_shared<MinDist>(MinDist());
    }

    static std::vector<std::shared_ptr<Attribute>> GenerateAttributes(const std::vector<std::string> &stringAttributes) {
        std::vector<std::shared_ptr<Attribute>> attributes;

        for (const auto &att : stringAttributes) {
            std::string name;
            for (uint i = 0; i < att.size(); i++)
                if (std::isalpha(att[i]))
                    name += att[i];
                else
                    break;

            if (name == "freq")
                attributes.push_back(GenerateFrequency(att));
            else if (name == "minDist")
                attributes.push_back(GenerateMinDist());
        }

        return attributes;
    }
}

#endif //FEATUREEXTRACTION_ATTRIBUTEBUILDER_H
