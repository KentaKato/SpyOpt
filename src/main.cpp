#include <iostream>

#include "SpyOpt/config_parser.h"
#include "SpyOpt/spy_opt.h"

using namespace spy;



int main()
{
    auto objective = [](const std::vector<double> &pos) -> double
    {
        double sum = 0.0;
        for (size_t i = 0, n = pos.size(); i < n; ++i)
        {
            sum += (pos[i] - i) * (pos[i] - i);
        }
        return sum;
    };

    Config config;
    if (!parseConfig("../resources/config.yaml", config))
    {
        std::cerr << "[Error] Failed to parse config!" << std::endl;
        return -1;
    }
    std::cout << config << std::endl;

    SpyOpt spy_alg(config, objective);
    spy_alg.optimize();
    const auto [fitness, pos] = spy_alg.getBestFitness();
    std::cout << "Best solution:" << std::endl;
    spy_alg.printBestAgent();

    return 0;
}