#ifndef SPY_OPT__AGENT_H
#define SPY_OPT__AGENT_H

#include <vector>
#include <functional>
#include <random>
#include <iostream>

namespace spy
{

class Agent
{

public:
    explicit Agent(const std::vector<double> init_pos,
                   std::function<double(const std::vector<double>&)> objective_func,
                   const std::vector<double> lower_bounds,
                   const std::vector<double> upper_bounds,
                   const std::mt19937 &rand_engine);
    double fitness;

    void swingMove(size_t time, double swing_factor);
    void moveToward(const Agent &better_agent);
    void randomSearch();
    const std::vector<double>& getPosition() const;

    friend bool operator<(const Agent& lhs, const Agent& rhs);
    friend std::ostream& operator<<(std::ostream& os, const Agent& agent);

private:
    void clipPosition();

    std::vector<double> position_;
    std::vector<double> lower_bounds_, upper_bounds_;
    std::function<double(const std::vector<double>&)> objective_func_;

    std::mt19937 rand_engine_;
};

} // namespace spy

#endif