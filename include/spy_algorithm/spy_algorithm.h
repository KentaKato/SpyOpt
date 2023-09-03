#ifndef SPY_ALGORITHM__SPY_ALGORITHM_H
#define SPY_ALGORITHM__SPY_ALGORITHM_H

#include <string>
#include <vector>
#include "spy_algorithm/agent.h"

namespace spy
{

class SpyAlgorithm
{

public:
    explicit SpyAlgorithm(const std::string &config_file,
                          std::function<double(const std::vector<double>&)> objective_func);
    void optimize();
    double getBestFitness() const;
    void printAgents() const;

private:
    [[nodiscard]] bool loadConfig(const std::string &config_filejk);
    void generateAgents(std::function<double(const std::vector<double>&)> objective_func);
    void sortAgents();

    std::vector<Agent> agents_;

    std::mt19937 rand_engine_;
    std::uniform_real_distribution<> uniform_dist_;

    struct Config
    {
        size_t num_agents;
        size_t num_iterations;
        double swing_factor;
        std::vector<double> lower_bounds, upper_bounds;
        size_t input_dim;
    };
    Config config_;



};

} // namespace spy

#endif