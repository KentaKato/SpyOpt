#ifndef SPY_OPT__AGENT_H
#define SPY_OPT__AGENT_H

#include <vector>
#include <functional>
#include <random>
#include <iostream>

namespace spy_opt
{

class Agent
{

public:
    explicit Agent(size_t id,
                   const std::vector<double> init_pos,
                   std::function<double(const std::vector<double>&)> objective_func,
                   const std::vector<double> lower_bounds,
                   const std::vector<double> upper_bounds,
                   size_t max_iteration,
                   const std::mt19937 &rand_engine);
    size_t id;
    double fitness;
    std::vector<std::vector<double>> pos_history;
    std::vector<double> fitness_history;

    void reset(const std::vector<double> &init_pos);
    void swingMove(size_t time, double swing_factor);
    void moveToward(const Agent &better_agent);
    void randomSearch();
    const std::vector<double>& getPosition() const;

    friend bool operator<(const Agent &lhs, const Agent &rhs);
    friend std::ostream& operator<<(std::ostream &os, const Agent &agent);

private:
    void clipPosition();

    std::vector<double> position_;
    std::vector<double> lower_bounds_, upper_bounds_;
    std::function<double(const std::vector<double>&)> objective_func_;

    std::mt19937 rand_engine_;
};

} // namespace spy_opt

#endif