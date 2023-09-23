#include <algorithm>
#include <fstream>
#include <iostream>
#include <iomanip> // for std::setw
#include <yaml-cpp/yaml.h>

#include "SpyOpt/spy_opt.h"

namespace spy_opt
{

std::ostream& operator<<(std::ostream &os, const Config &config)
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
    os << "\n  upper_bounds: ";
    print_vec(config.upper_bounds);
    return os;
}

/* Public methods */

SpyOpt::SpyOpt(const Config &config,
               std::function<double(const std::vector<double>&)> objective_func)
               : config_(config), uniform_dist_(0., 1.)
{
    this->validateConfig();
    std::random_device rd;
    rand_engine_.seed(rd());
    this->generateAgents(objective_func);

    best_fitness_history_.reserve(config_.num_iterations);
    best_pos_history_.reserve(config_.num_iterations);
    this->updateHistory();
}

void SpyOpt::optimize()
{
    static const size_t num_high_mid = config_.num_high_rank + config_.num_mid_rank;
    for(size_t t = 1; t < config_.num_iterations; ++t)
    {
        for(size_t i = 0; i < config_.num_high_rank; ++i)
        {
            agents_[i].swingMove(t, config_.swing_factor);
        }
        for(size_t i = config_.num_high_rank; i < num_high_mid; ++i)
        {
            std::uniform_int_distribution<> rand(0, i-1);
            agents_[i].moveToward(agents_[rand(rand_engine_)]);
        }
        for(size_t i = num_high_mid; i < config_.num_agents; ++i)
        {
            agents_[i].randomSearch();
        }
        this->sortAgentsByFitness();
        this->printProgress(t);
        this->updateHistory();
    }
}

void SpyOpt::reset()
{
    last_printed_progress_ = 0;
    best_fitness_history_.clear();
    best_pos_history_.clear();
    for (auto &agent : agents_)
    {
        agent.reset(this->generateRandomPosition());
    }
    this->updateHistory();
}

std::pair<double, std::vector<double>> SpyOpt::getBestFitness() const
{
    if (agents_.empty())
    {
        throw std::runtime_error("No agents available!");
    }
    const Agent &best_agent = agents_.front();
    return {best_agent.fitness, best_agent.getPosition()};
}

void SpyOpt::printAgents() const
{
    for (const auto &agent : agents_)
    {
        std::cout << "  " << agent << std::endl;
    }
}

void SpyOpt::printBestAgent() const
{
    std::cout << agents_.front() << std::endl;
}

void SpyOpt::dumpBestSolutionHistory(const std::string &filename)
{
    std::ofstream file(filename);

    // header
    file << "iteration,fitness";
    for (size_t itr = 0; itr < config_.input_dim; ++itr)
    {
        file << ",x" << itr;
    }
    file << "\n";

    const size_t max_itr = best_pos_history_.size();
    for (size_t itr = 0; itr < max_itr; ++itr)
    {
        file << itr;
        file << ", " << best_fitness_history_.at(itr);
        for (const auto &pi : best_pos_history_.at(itr))
        {
            file << ", " << pi;
        }
        file << "\n";
    }
}

void SpyOpt::dumpAgentsHistory(const std::string &filename)
{
    std::ofstream file(filename);

    // header
    file << "iteration,agent_id,fitness";
    for (size_t itr = 0; itr < config_.input_dim; ++itr)
    {
        file << ",x" << itr;
    }
    file << "\n";

    this->sortAgentsByID();
    const size_t max_itr = agents_.at(0).pos_history.size();
    for (size_t itr = 0; itr < max_itr; ++itr)
    {
        for (const auto &agent : agents_)
        {
            file << itr;
            file << ", " << agent.id;
            file << ", " << agent.fitness_history.at(itr);
            for (const auto &pi : agent.pos_history.at(itr))
            {
                file << ", " << pi;
            }
            file << "\n";
        }
    }
}

/* Private methods */

void SpyOpt::generateAgents(std::function<double(const std::vector<double>&)> objective_func)
{
    agents_.reserve(config_.num_agents);
    for(size_t i = 0; i < config_.num_agents; ++i)
    {
        agents_.emplace_back(i,
                             this->generateRandomPosition(),
                             objective_func,
                             config_.lower_bounds,
                             config_.upper_bounds,
                             config_.num_iterations,
                             rand_engine_);
    }
    this->sortAgentsByFitness();
}

std::vector<double> SpyOpt::generateRandomPosition()
{
    std::vector<double> pos(config_.input_dim);
    for (size_t i = 0, n = pos.size(); i < n; ++i)
    {
        const double range = config_.upper_bounds[i] - config_.lower_bounds[i];
        pos[i] = config_.lower_bounds[i] + range * uniform_dist_(rand_engine_);
    }
    return pos;
}

void SpyOpt::sortAgentsByFitness()
{
    std::sort(agents_.begin(),
              agents_.end(),
              [](const Agent &lhs, const Agent &rhs) -> bool
              {
                  return lhs.fitness < rhs.fitness;
              });
}

void SpyOpt::sortAgentsByID()
{
    std::sort(agents_.begin(),
              agents_.end(),
              [](const Agent &lhs, const Agent &rhs) -> bool
              {
                  return lhs.id < rhs.id;
              });
}

void SpyOpt::validateConfig() const
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

void SpyOpt::printProgress(size_t iteration)
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
        auto precision_backup = std::cout.precision();
        std::cout << "] " << std::setw(5) << std::fixed << std::setprecision(1) << progress * 100.0 << "%\r";
        std::cout.precision(precision_backup);
        std::cout.flush();
        last_printed_progress_ = iteration;
    }

    // New line when 100%
    if(iteration == config_.num_iterations)
    {
        std::cout << std::endl;
    }
}

void SpyOpt::updateHistory()
{
    best_fitness_history_.emplace_back(agents_.front().fitness);
    best_pos_history_.emplace_back(agents_.front().getPosition());
}

} // namespace spy_opt