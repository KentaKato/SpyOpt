#ifndef SPY_ALGORITHM__AGENT_H
#define SPY_ALGORITHM__AGENT_H

#include <vector>
#include <functional>
#include <random>

namespace spy
{

class Agent
{

public:
    Agent(const std::vector<double> init_pos,
          std::function<double(const std::vector<double>&)> objective_func);

    double fitness;

    void swingMove(size_t time, double swing_factor);
    void moveToward(const Agent &better_agent);

    friend bool operator<(const Agent& lhs, const Agent& rhs);

private:
    size_t dimension_;
    std::vector<double> position_;
    std::function<double(const std::vector<double>&)> objective_func_;

};

} // namespace spy

#endif