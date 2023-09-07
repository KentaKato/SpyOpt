#include <iostream>

#include "SpyOpt/config_parser.h"
#include "SpyOpt/spy_opt.h"

using namespace spy_opt;

int main()
{
    // Booth function: f(1, 3)=0
    auto booth_func = [](const std::vector<double> &pos) -> double
    {
        double x = pos[0];
        double y = pos[1];
        return std::pow(x + 2. * y - 7., 2.) + std::pow(2. * x + y - 5., 2.);
    };

    // Eggholder function: f(512, 404.2319)=-959.6407
    auto eggholder_func = [](const std::vector<double> &pos) -> double
    {
        double x = pos[0];
        double y = pos[1];
        return -(y + 47) * sin(sqrt(abs(x/2 + (y+47))))
            - x * sin(sqrt(abs(x - (y+47))));
    };

    Config config;
    if (!parseConfig("../resources/config.yaml", config))
    {
        std::cerr << "[Error] Failed to parse config!" << std::endl;
        return -1;
    }
    std::cout << config << std::endl;

    SpyOpt spy_alg(config, eggholder_func);
    spy_alg.optimize();
    const auto [fitness, pos] = spy_alg.getBestFitness();
    std::cout << "Best solution:" << std::endl;
    spy_alg.printBestAgent();
    spy_alg.dumpHistory("../results/history.csv");

    return 0;
}