#ifndef FEATUREEXTRACTION_ATTRIBUTE_H
#define FEATUREEXTRACTION_ATTRIBUTE_H

#include <cassert>
#include <map>
#include <optional>
#include "misc/Constants.hpp"
#include "types/Series.hpp"

class Attribute {
public:
    [[nodiscard]] virtual inline std::string Name() const = 0;
    [[nodiscard]] virtual inline std::string Param1() const { return ""; };
    [[nodiscard]] virtual inline std::string Param2() const { return ""; };
    [[nodiscard]] virtual inline std::string Param3() const { return ""; };
    [[nodiscard]] virtual double GenerateValue(const Series &series, const Series &window) const = 0;
};

#endif //FEATUREEXTRACTION_ATTRIBUTE_H
