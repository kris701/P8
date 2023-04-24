import configparser

def ParseConfigIntoObject(configName : str, sectionName : str, obj) -> None:
    config = configparser.RawConfigParser()
    config.optionxform = lambda option: option
    config.read(configName)

    if config.has_section("INCLUDE"):
        for includeItem in config["INCLUDE"]:
            ParseConfigIntoObject(config["INCLUDE"][includeItem], sectionName, obj)

    if config.has_section(sectionName):
        for index in config[sectionName]:
            if index in obj.__annotations__:
                typeName = obj.__annotations__[index].__name__;
                if typeName == "str":
                    obj.__dict__[index] = config[sectionName][index]
                elif typeName == "bool":
                    obj.__dict__[index] = config[sectionName].getboolean(index)
                elif typeName == "int":
                    obj.__dict__[index] = config[sectionName].getint(index)
                elif typeName == "float":
                    obj.__dict__[index] = config[sectionName].getfloat(index)
                elif typeName == "list":
                    items = config[sectionName][index].split(",")
                    obj.__dict__[index] = items;
                else:
                    raise Exception("Invalid config type!")
            else:
                raise Warning("Warning, key '" + index + "' not in this object!")