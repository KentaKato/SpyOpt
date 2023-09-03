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
    SpyAlgorithm(const std::string &config_file,
                 std::function<double(const std::vector<double>&)> objective_func);
    void optimize();
    double getBestFitness() const;

private:
    [[nodiscard]] bool loadConfig(const std::string &config_filejk);
    void generateAgents();

    std::vector<Agent> agents_;
    std::function<double(const std::vector<double>&)> objective_func_;

    std::mt19937 rand_engine_;
    std::uniform_real_distribution<> uniform_dist_;

    struct Config
    {
        size_t num_agents;
        size_t num_iterations;
        double swing_factor;
        size_t input_dim;
        double lower_bound, upper_bound;
    };
    Config config_;



};

} // namespace spy

#endif