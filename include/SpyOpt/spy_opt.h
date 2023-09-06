#ifndef SPY_OPT__SPY_OPT_H
#define SPY_OPT__SPY_OPT_H

#include <string>
#include <vector>
#include "SpyOpt/agent.h"

namespace spy_opt
{

struct Config
{
    size_t num_agents;
    size_t num_high_rank, num_mid_rank;
    size_t num_iterations;
    double swing_factor;
    std::vector<double> lower_bounds, upper_bounds;
    size_t input_dim;
};
std::ostream& operator<<(std::ostream& os, const Config& config);

class SpyOpt
{

public:
    explicit SpyOpt(const Config &config,
                          std::function<double(const std::vector<double>&)> objective_func);
    void optimize();

    // return [fitness, position]
    std::pair<double, std::vector<double>> getBestFitness() const;
    void printAgents() const;
    void printBestAgent() const;
    void dumpHistory(const std::string &filename);

private:
    void generateAgents(std::function<double(const std::vector<double>&)> objective_func);
    void sortAgentsByFitness();
    void sortAgentsByID();
    void validateConfig() const;
    std::vector<double> generateRandomPosition();
    void printInitialConditions() const;
    void printFinalConditions() const;
    void printProgress(size_t iteration);

    std::vector<Agent> agents_;

    std::mt19937 rand_engine_;
    std::uniform_real_distribution<> uniform_dist_;

    Config config_;
    size_t last_printed_progress_ = 0;
};

} // namespace spy_opt

#endif