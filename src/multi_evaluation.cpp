#include <iostream>

#include "SpyOpt/config_parser.h"
#include "SpyOpt/spy_opt.h"
#include "SpyOpt/objective_functions.h"

using namespace spy_opt;

int main()
{
    size_t MAX_EPOCH = 300;

    Config config;
    if (!parseConfig("../resources/config.yaml", config))
    {
        std::cerr << "[Error] Failed to parse config!" << std::endl;
        return -1;
    }
    std::cout << config << std::endl;

    std::function<double(const std::vector<double>&)> objective_function;
    if (config.objective_func_name == "Booth")
    {
        std::cout << "Using Booth function." << std::endl;
        objective_function = booth_func;
    }
    else if (config.objective_func_name == "Eggholder")
    {
        std::cout << "Using Eggholder function." << std::endl;
        objective_function = eggholder_func;
    }
    else if (config.objective_func_name == "Ackley")
    {
        std::cout << "Using Ackley function." << std::endl;
        objective_function = ackley_function;
    }
    else
    {
        std::cerr << "[Error] Invalid objective function name." << std::endl;
        return -1;
    }

    SpyOpt spy_alg(config, objective_function);
    for (size_t epoch = 0; epoch < MAX_EPOCH; ++epoch)
    {
        spy_alg.reset();
        spy_alg.optimize();
        const auto [fitness, pos] = spy_alg.getBestFitness();
        spy_alg.dumpBestSolutionHistory("../results/multi_evaluation/best_solution_history_" + std::to_string(epoch) + ".csv");
        spy_alg.dumpAgentsHistory("../results/multi_evaluation/agents_history_" + std::to_string(epoch) + ".csv");
    }

    return 0;
}