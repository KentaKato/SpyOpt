#ifndef SPY_ALGORITHM__YAML_UTILS_H
#define SPY_ALGORITHM__YAML_UTILS_H

#include <yaml-cpp/yaml.h>

namespace spy
{

template <typename T>
bool safeLoadScalar(const YAML::Node &node, const std::string &key, T &value)
{
    if (node[key] && node[key].IsScalar())
    {
        value = node[key].as<T>();
        return true;
    }
    std::cerr << "[Error] Invalid or missing '" << key << "' in config." << std::endl;
    return false;
}

template <typename T>
bool safeLoadVector(const YAML::Node& node, const std::string& key, std::vector<T>& vec)
{
    if (node[key] && node[key].IsSequence())
    {
        for (const auto& item : node[key])
        {
            if (!item.IsScalar())
            {
                std::cerr << "[Error] Invalid number in sequence '" << key << "'." << std::endl;
                return false;
            }
        }
        vec = node[key].as<std::vector<T>>();
        return true;
    }
    std::cerr << "[Error] Invalid or missing sequence '" << key << "' in config." << std::endl;
    return false;
}

} // namespace spy

#endif