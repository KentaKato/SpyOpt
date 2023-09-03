#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <yaml-cpp/yaml.h>

#include "spy_algorithm/spy_algorithm.h"
#include "spy_algorithm/yaml_utils.h"

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
    this->generateAgents(objective_func);
}

void SpyAlgorithm::optimize()
{
    for(int t = 1; t < config_.num_iterations; ++t)
    {
        for(auto &agent : agents_) {
            if(&agent == &agents_.front())
            {
                agent.swingMove(t, config_.swing_factor);
            } else {
                // int betterAgentIndex = std::rand() % (&agent - &agents.front());
                // agent.moveToward(agents[betterAgentIndex]);
            }
        }
        this->sortAgents();

        auto print_progress = [&]
        {
            if (t % (config_.num_iterations / 10) == 0)
            {
                std::cout<< double(t) / config_.num_iterations * 100 << "%" << std::endl;
            }
        };
        print_progress();

    }
    std::cout << "Final conditions:" << std::endl;
    this->printAgents();
}

double SpyAlgorithm::getBestFitness() const
{
    return agents_.front().fitness;
}

void SpyAlgorithm::printAgents() const
{
    std::for_each(
        agents_.begin(),
        agents_.end(),
        [](const Agent &a){std::cout << "  " << a << std::endl;});
}

void SpyAlgorithm::generateAgents(std::function<double(const std::vector<double>&)> objective_func)
{
    auto rand_pos = [&]()
    {
        std::vector<double> pos(config_.input_dim);
        for (size_t i = 0, n = pos.size(); i < n; ++i)
        {
            const double range = config_.upper_bounds[i] - config_.lower_bounds[i];
            pos[i] = config_.lower_bounds[i] + range * uniform_dist_(rand_engine_);
        }
        return pos;
    };

    agents_.reserve(config_.num_agents);
    for(size_t i = 0; i < config_.num_agents; ++i)
    {
        agents_.emplace_back(rand_pos(),
                             objective_func,
                             config_.lower_bounds,
                             config_.upper_bounds,
                             rand_engine_);
    }
    this->sortAgents();

    std::cout << "Initial conditions:" << std::endl;
    this->printAgents();
}

void SpyAlgorithm::sortAgents()
{
    std::sort(agents_.begin(), agents_.end());
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
            !safeLoadVector(c, "lower_bounds", config_.lower_bounds) ||
            !safeLoadVector(c, "upper_bounds", config_.upper_bounds))
        {
            return false;
        }
        config_.input_dim = config_.lower_bounds.size();

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

        if (config_.lower_bounds.size() != config_.upper_bounds.size())
        {
            std::cerr << "[Error] 'lower_bounds' and 'upper_bounds' should have the same length." << std::endl;
            return false;
        }

        // Ensure upper_bound > lower_bound
        for (size_t i = 0, n = config_.lower_bounds.size(); i < n; ++i)
        {
            if (config_.upper_bounds[i] <= config_.lower_bounds[i])
            {
                std::cerr << "[Error] 'upper_bounds' should be greater than 'lower_bounds'." << std::endl;
                return false;
            }
        }
    }
    catch (const YAML::Exception& e)
    {
        std::cerr << "[Error] Failed to parse the config file: " << e.what() << std::endl;
        return false;
    }
    return true;
}


} // namespace spy