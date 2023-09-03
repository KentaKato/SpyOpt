#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <yaml-cpp/yaml.h>

#include "spy_algorithm/spy_algorithm.h"

namespace spy
{

SpyAlgorithm::SpyAlgorithm(const std::string& config_file,
                           std::function<double(const std::vector<double>&)> objective_func)
                           : uniform_dist_(0., 1.)
{
    if (!this->loadConfig(config_file))
    {
        throw std::invalid_argument("Failed to load the config file: " + config_file);
    }

    std::random_device rd;
    rand_engine_.seed(rd());
    this->generateAgents();




}


void SpyAlgorithm::generateAgents()
{
    agents_.reserve(config_.num_agents);
    for(size_t i = 0; i < config_.num_agents; ++i)
    {

        uniform_dist_(rand_engine_)
        agents_.emplace_back(D, objective_func_);
    }
}

bool SpyAlgorithm::loadConfig(const std::string &config_file)
{
    if (!std::filesystem::exists(config_file))
    {
        std::cerr << "[Error] Config file does not exist: " << config_file << std::endl;
        return false;
    }

    try
    {
        YAML::Node c = YAML::LoadFile(config_file);

        if (!safeLoadScalar(c, "num_agents", config_.num_agents) ||
            !safeLoadScalar(c, "num_iterations", config_.num_iterations) ||
            !safeLoadScalar(c, "swing_factor", config_.swing_factor) ||
            !safeLoadScalar(c, "lower_bound", config_.lower_bound) ||
            !safeLoadScalar(c, "lower_bound", config_.lower_bound) ||
            !safeLoadScalar(c, "input_dim", config_.input_dim))
        {
            return false;
        }

        /* Validation */
        if (config_.num_agents <= 0)
        {
            std::cerr << "[Error] 'num_agent' should be greater than zero." << std::endl;
            return false;
        }

        if (config_.num_iterations <= 0)
        {
            std::cerr << "[Error] 'num_iteration' should be greater than zero." << std::endl;
            return false;
        }

        if (config_.input_dim <= 0)
        {
            std::cerr << "[Error] 'input_dim' should be greater than zero." << std::endl;
            return false;
        }

        // Ensure upper_bound > lower_bound
        if (config_.upper_bound <= config_.lower_bound)
        {
            std::cerr << "[Error] 'upper_bound' should be greater than 'lower_bound'." << std::endl;
            return false;
        }
    }
    catch (const YAML::Exception& e)
    {
        std::cerr << "[Error] Failed to parse the config file: " << e.what() << std::endl;
        return false;
    }
    return true;
}

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

} // namespace spy