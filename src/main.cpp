#include <iostream>

#include "spy_algorithm/spy_algorithm.h"

using namespace spy;



int main()
{
    auto objective = [](const std::vector<double> &pos) -> double
    {
        double sum = 0.0;
        for(const auto &val : pos)
        {
            sum += val * val;
        }
        return sum;
    };

    SpyAlgorithm spy_alg("../resources/config.yaml", objective);
    spy_alg.optimize();

    return 0;
}