#include <iostream>

#include "SpyOpt/config_parser.h"
#include "SpyOpt/spy_opt.h"

using namespace spy_opt;

int main()
{
    // Booth function: f(1, 3)=0
    auto objective = [](const std::vector<double> &pos) -> double
    {
        double x = pos[0];
        double y = pos[1];
        // return std::pow(x + 2. * y - 7., 2.) + std::pow(2. * x + y - 5., 2.);
        return (x + 2. * y - 7.)*(x + 2. * y - 7.) + (2. * x + y - 5.)*(2. * x + y - 5.);
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
    spy_alg.dumpHistory("../results/history.csv");

    return 0;
}