#ifndef SPY_OPT__OBJECTIVEFUNCTIONS_H
#define SPY_OPT__OBJECTIVEFUNCTIONS_H

#include <vector>
#include <cmath>

namespace spy_opt {

    // Booth function: f(1, 3)=0
    double booth_func(const std::vector<double> &pos)
    {
        const double x = pos[0];
        const double y = pos[1];
        return std::pow(x + 2. * y - 7., 2.) + std::pow(2. * x + y - 5., 2.);
    }

    // Eggholder function: f(512, 404.2319)=-959.6407
    double eggholder_func(const std::vector<double> &pos)
    {
        const double x = pos[0];
        const double y = pos[1];
        return -(y + 47) * sin(sqrt(abs(x/2 + (y+47))))
            - x * sin(sqrt(abs(x - (y+47))));
    }

    // Ackley function: f(0, 0)=0
    double ackley_function(const std::vector<double> &pos)
    {
        const double x = pos[0];
        const double y = pos[1];
        const double a = 20;
        const double b = 0.2;
        const double c = 2 * M_PI;
        const double sum1 = x*x + y*y;
        const double sum2 = cos(c*x) + cos(c*y);
        return -a * exp(-b*sqrt(0.5*sum1)) - exp(0.5*sum2) + a + exp(1);
    }

} // namespace spy_opt


#endif