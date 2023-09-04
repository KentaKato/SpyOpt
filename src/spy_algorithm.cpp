#include <algorithm>
#include <fstream>
#include <iostream>
#include <yaml-cpp/yaml.h>

#include "spy_algorithm/spy_algorithm.h"

namespace spy
{

/* Public methods */

SpyAlgorithm::SpyAlgorithm(const Config &config,
                           std::function<double(const std::vector<double>&)> objective_func)
                           : config_(config), uniform_dist_(0., 1.)
{
    if (!this->validateConfig())
    {
        throw std::invalid_argument("Invalid config is given!");
    }
    std::random_device rd;
    rand_engine_.seed(rd());
    this->generateAgents(objective_func);
}

void SpyAlgorithm::optimize()
{
    static const size_t num_high_mid = config_.num_high_rank + config_.num_mid_rank;

    for(size_t t = 1; t < config_.num_iterations; ++t)
    {
        for(size_t i = 0, n = agents_.size(); i < n; ++i)
        {
            if(i < config_.num_high_rank)
            {
                agents_[i].swingMove(t, config_.swing_factor);
            }
            else if (i < num_high_mid)
            {
                std::uniform_int_distribution<> rand(0, i-1);
                agents_[i].moveToward(agents_[rand(rand_engine_)]);
            }
            else
            {
                agents_[i].randomSearch();
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

std::tuple<double, std::vector<double>> SpyAlgorithm::getBestFitness() const
{
    return {agents_.front().fitness, agents_.front().getPosition()};
}

void SpyAlgorithm::printAgents() const
{
    std::for_each(
        agents_.begin(),
        agents_.end(),
        [](const Agent &a){std::cout << "  " << a << std::endl;});
}

/* Private methods */

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

bool SpyAlgorithm::validateConfig()
{
        bool ret = true;
        if (config_.num_agents <= 0 || config_.num_high_rank <= 0 || config_.num_mid_rank <= 0)
        {
            std::cerr
                << "[Error] 'num_agent', 'num_hign_rank' and 'num_mid_rank' should be "
                << "greater than zero." << std::endl;
            ret = false;
        }
        if (config_.num_agents <= config_.num_high_rank + config_.num_mid_rank)
        {
            std::cerr
                << "[Error] 'num_agent' should be greater than (num_high_rank + num_mid_rank)."
                << std::endl;
            ret = false;
        }
        if (config_.num_iterations <= 0)
        {
            std::cerr << "[Error] 'num_iteration' should be greater than zero." << std::endl;
            ret = false;
        }
        if (config_.lower_bounds.size() != config_.upper_bounds.size())
        {
            std::cerr
                << "[Error] 'lower_bounds' and 'upper_bounds' should have the same length."
                << std::endl;
            ret = false;
        }
        // Ensure upper_bound > lower_bound
        for (size_t i = 0, n = config_.lower_bounds.size(); i < n; ++i)
        {
            if (config_.upper_bounds[i] <= config_.lower_bounds[i])
            {
                std::cerr << "[Error] 'upper_bounds' should be greater than 'lower_bounds'." << std::endl;
                ret = false;
            }
        }
        return ret;
}

} // namespace spy