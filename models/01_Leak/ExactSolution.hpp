#include <math.h>

namespace
{
    double const GAS_CONSTANT = 8.314462; // Universal gas constant [J/mol/K]
}

// Analytical solution to the leaking tank problem
// https://physics.stackexchange.com/a/631929/339039
class ExactSolution
{
public:
    // leakArea - Leak area [m^2]
    // tankVolume - Tank volume [m^3]
    // initialPressure - Initial tank pressure [kPa]
    // initialTemperature - Initial tank temperature [K]
    // molarWeight - Molar weight of the gas [kg/mol]
    ExactSolution(double leakArea, double tankVolume, double initialPressure, double initialTemperature, double molarWeight)
        : mInitialPressure(initialPressure)
    {
        mExponentialBase = std::exp(-leakArea / tankVolume * std::sqrt(2 * GAS_CONSTANT * initialTemperature / molarWeight));
    }

    ~ExactSolution() = default;

    // time - [s]
    double getPressure(double time)
    {
        return mInitialPressure * std::pow(mExponentialBase, time);
    }

private:
    double mInitialPressure;
    double mExponentialBase;
};