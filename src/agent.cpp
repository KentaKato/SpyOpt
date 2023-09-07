#include "SpyOpt/agent.h"

namespace spy_opt
{

Agent::Agent(size_t id,
             const std::vector<double> init_pos,
             std::function<double(const std::vector<double>&)> objective_func,
             const std::vector<double> lower_bounds,
             const std::vector<double> upper_bounds,
             size_t max_iteration,
             const std::mt19937 &rand_engine)
    : id(id),
      position_(init_pos),
      objective_func_(objective_func),
      lower_bounds_(lower_bounds),
      upper_bounds_(upper_bounds),
      rand_engine_(rand_engine),
      fitness(objective_func(init_pos))
{
    pos_history.reserve(max_iteration);
    pos_history.emplace_back(position_);

    fitness_history.reserve(max_iteration);
    fitness_history.emplace_back(fitness);
}

void Agent::swingMove(size_t time, double swing_factor)
{
    static std::uniform_real_distribution<> uniform_dist(-1, 1);
    for(auto &pos : position_) {
        pos += uniform_dist(rand_engine_) * (swing_factor / time);
    }
    this->clipPosition();
    fitness = objective_func_(position_);
    pos_history.emplace_back(position_);
    fitness_history.emplace_back(fitness);
}

void Agent::moveToward(const Agent &better_agent)
{
    static std::uniform_real_distribution<> uniform_dist(-1, 1);
    for(size_t i = 0, n = position_.size(); i < n; ++i)
    {
        position_[i] += uniform_dist(rand_engine_) * (better_agent.getPosition()[i] - position_[i]);
    }
    this->clipPosition();
    fitness = objective_func_(position_);
    pos_history.emplace_back(position_);
    fitness_history.emplace_back(fitness);
}

void Agent::randomSearch()
{
    static std::uniform_real_distribution<> uniform_dist(0, 1);
    for (size_t i = 0, n = position_.size(); i < n; ++i)
    {
        const double range = upper_bounds_[i] - lower_bounds_[i];
        position_[i] = lower_bounds_[i] + range * uniform_dist(rand_engine_);
    }
    this->clipPosition();
    fitness = objective_func_(position_);
    pos_history.emplace_back(position_);
    fitness_history.emplace_back(fitness);
}

const std::vector<double>& Agent::getPosition() const
{
    return position_;
}

void Agent::clipPosition()
{
    for (size_t i = 0, n = position_.size(); i < n; ++i)
    {
        position_[i] = std::clamp(position_[i], lower_bounds_[i], upper_bounds_[i]);
    }
}

bool operator<(const Agent &lhs, const Agent &rhs)
{
    return lhs.fitness < rhs.fitness;
}

std::ostream& operator<<(std::ostream& os, const Agent& agent)
{
    os << "Agent ID: " << agent.id;
    os << ", pos: [";
    for (auto it = agent.position_.begin(); it != agent.position_.end(); ++it)
    {
        if (it != agent.position_.begin())
        {
            os << ", ";
        }
        os << *it;
    }
    os << "]";
    os << ", fitness: " << agent.fitness;
    return os;
}

} // namespace spy_opt