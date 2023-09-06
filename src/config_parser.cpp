#include <iostream>
#include <filesystem>

#include "SpyOpt/spy_opt.h"
#include "SpyOpt/config_parser.h"

namespace spy_opt
{

[[nodiscard]] bool parseConfig(const std::string &config_path, Config &config)
{
    if (!std::filesystem::exists(config_path))
    {
        std::cerr << "[Error] Config file does not exist: " << config_path << std::endl;
        return false;
    }
    try
    {
        YAML::Node node = YAML::LoadFile(config_path);

        if (!safeLoadScalar(node, "num_agents", config.num_agents) ||
            !safeLoadScalar(node, "num_high_rank", config.num_high_rank) ||
            !safeLoadScalar(node, "num_mid_rank", config.num_mid_rank) ||
            !safeLoadScalar(node, "num_iterations", config.num_iterations) ||
            !safeLoadScalar(node, "swing_factor", config.swing_factor) ||
            !safeLoadVector(node, "lower_bounds", config.lower_bounds) ||
            !safeLoadVector(node, "upper_bounds", config.upper_bounds))
        {
            return false;
        }
        config.input_dim = config.lower_bounds.size();
    }
    catch (const YAML::Exception& e)
    {
        std::cerr << "[Error] Failed to parse the config file: " << e.what() << std::endl;
        return false;
    }
    return true;
}

} // namespace spy_opt
