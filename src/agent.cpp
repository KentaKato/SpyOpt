#include "spy_algorithm/agent.h"

namespace spy
{

Agent::Agent(const std::vector<double> init_pos,
             std::function<double(const std::vector<double>&)> objective_func)
    : position_(init_pos),
      objective_func_(objective_func),
      fitness(std::numeric_limits<double>::max()) {}

void Agent::swingMove(size_t time, double swing_factor)
{
    std::uniform_real_distribution<> dis(-1, 1);

    for(auto &pos : position_) {
        pos += dis(gen) * (swing_factor / time);
    }

    fitness = objective_func_(position_);
}

void Agent::moveToward(const Agent &better_agent)
{
    std::uniform_real_distribution<> dis(-1, 1);

    for(int i = 0; i < dimension_; ++i) {
        position_[i] += dis(gen) * (better_agent.position_[i] - position_[i]);
    }

    fitness = objective_func_(position_);
}

bool operator<(const Agent &lhs, const Agent &rhs)
{
    return lhs.fitness < rhs.fitness;
}

} // namespace spy