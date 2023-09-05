#include <algorithm>
#include <fstream>
#include <iostream>
#include <iomanip> // for std::setw
#include <yaml-cpp/yaml.h>

#include "spy_algorithm/spy_algorithm.h"

namespace spy
{

std::ostream& operator<<(std::ostream& os, const Config& config)
{
    auto print_vec = [&](const auto &vec)
    {
        os << "[";
        for (auto it = vec.begin(); it != vec.end(); ++it)
        {
            if (it != vec.begin())
            {
                os << ", ";
            }
            os << *it;
        }
        os << "]";
    };

    os << "Config:";
    os << "\n  num_agents: " << config.num_agents;
    os << "\n  num_high_rank: " << config.num_high_rank;
    os << "\n  num_mid_rank: " << config.num_mid_rank;
    os << "\n  num_iterations: " << config.num_iterations;
    os << "\n  swing_factor: " << config.swing_factor;
    os << "\n  input_dim: " << config.input_dim;
    os << "\n  lower_bounds: ";
    print_vec(config.lower_bounds);
    os << "\n upper_bounds: ";
    print_vec(config.upper_bounds);
    return os;
}

/* Public methods */

SpyAlgorithm::SpyAlgorithm(const Config &config,
                           std::function<double(const std::vector<double>&)> objective_func)
                           : config_(config), uniform_dist_(0., 1.)
{
    this->validateConfig();
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
        this->printProgress(t);
    }
    // std::cout << "Final conditions:" << std::endl;
    // printAgents();
}

std::pair<double, std::vector<double>> SpyAlgorithm::getBestFitness() const
{
    if (agents_.empty())
    {
        throw std::runtime_error("No agents available!");
    }
    const Agent &best_agent = agents_.front();
    return {best_agent.fitness, best_agent.getPosition()};
}

void SpyAlgorithm::printAgents() const
{
    for(const auto& agent : agents_)
    {
        std::cout << "  " << agent << std::endl;
    }
}

void SpyAlgorithm::printBestAgent() const
{
    std::cout << agents_.front() << std::endl;
}

/* Private methods */

void SpyAlgorithm::generateAgents(std::function<double(const std::vector<double>&)> objective_func)
{
    agents_.reserve(config_.num_agents);
    for(size_t i = 0; i < config_.num_agents; ++i)
    {
        agents_.emplace_back(this->generateRandomPosition(),
                             objective_func,
                             config_.lower_bounds,
                             config_.upper_bounds,
                             rand_engine_);
    }
    this->sortAgents();

    // std::cout << "Initial conditions:" << std::endl;
    // printAgents();
}

std::vector<double> SpyAlgorithm::generateRandomPosition()
{
    std::vector<double> pos(config_.input_dim);
    for (size_t i = 0, n = pos.size(); i < n; ++i)
    {
        const double range = config_.upper_bounds[i] - config_.lower_bounds[i];
        pos[i] = config_.lower_bounds[i] + range * uniform_dist_(rand_engine_);
    }
    return pos;
}

void SpyAlgorithm::sortAgents()
{
    std::sort(agents_.begin(), agents_.end());
}

void SpyAlgorithm::validateConfig() const
{
        if (config_.num_agents <= 0 || config_.num_high_rank <= 0 || config_.num_mid_rank <= 0)
        {
            throw std::runtime_error(
                "[Error] 'num_agent', 'num_hign_rank' and 'num_mid_rank' should be greater than zero.");
        }
        if (config_.num_agents <= config_.num_high_rank + config_.num_mid_rank)
        {
            throw std::runtime_error(
                "[Error] 'num_agent' should be greater than (num_high_rank + num_mid_rank).");
        }
        if (config_.num_iterations <= 0)
        {
            throw std::runtime_error(
                "[Error] 'num_iteration' should be greater than zero.");
        }
        if (config_.lower_bounds.size() != config_.upper_bounds.size())
        {
            throw std::runtime_error(
                "[Error] 'lower_bounds' and 'upper_bounds' should have the same length.");
        }
        // Ensure upper_bound > lower_bound
        for (size_t i = 0, n = config_.lower_bounds.size(); i < n; ++i)
        {
            if (config_.upper_bounds[i] <= config_.lower_bounds[i])
            {
                throw std::runtime_error(
                    "[Error] 'upper_bounds' should be greater than 'lower_bounds'.");
            }
        }
}

void SpyAlgorithm::printProgress(size_t iteration)
{
    // convert zero-origin to one-origin
    iteration += 1;

    // width of the progress bar
    const int progress_width = 50;
    const double progress = double(iteration) / config_.num_iterations;

    if(iteration - last_printed_progress_ > config_.num_iterations / 100 || iteration == config_.num_iterations)
    {
        int position = static_cast<int>(progress_width * progress);
        std::cout << "[";
        for (int i = 0; i < progress_width; ++i)
        {
            if (i < position)
            {
                std::cout << "=";
            }
            else if (i == position)
            {
                std::cout << ">";
            }
            else
            {
                std::cout << " ";
            }
        }
        auto old_precision = std::cout.precision();
        std::cout << "] " << std::setw(5) << std::fixed << std::setprecision(1) << progress * 100.0 << "%\r";
        std::cout.precision(old_precision);
        std::cout.flush();
        last_printed_progress_ = iteration;
    }

    // New line when 100%
    if(iteration == config_.num_iterations)
    {
        std::cout << std::endl;
    }
}

} // namespace spy