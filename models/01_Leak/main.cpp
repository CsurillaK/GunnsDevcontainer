#include "gen/LeakNetwork.hh"
#include "matplotlib-cpp/matplotlibcpp.h"
#include "ExactSolution.hpp"

#include <iostream>

namespace plt = matplotlibcpp;

struct Data
{
    std::vector<double> Time;              // [s]
    std::vector<double> SimulatedPressure; // [kPa]
    std::vector<double> ExactPressure;     // [kPa]
};

void log(LeakNetwork const &network, const double time, Data &data, const double exactPressure)
{
    std::cout << "[" << time << "s] "
              << "Leak flowrate: " << network.leak.getFlowRate() << " kg/s | "
              << "Tank gas mass: " << network.tank.getNodeContent()->getMass() << " kg | "
              << "Tank pressure: " << network.tank.getNodeContent()->getPressure() << " kPa | "
              << "Tank O2 mole ratio: " << network.tank.getNodeContent()->getMoleFraction(FluidProperties::GUNNS_O2) * 100 << " % | " << std::endl;

    data.Time.push_back(time);
    data.SimulatedPressure.push_back(network.tank.getNodeContent()->getPressure());
    data.ExactPressure.push_back(exactPressure);
}

void plot(Data const &data)
{
    plt::figure();
    plt::named_plot("Simulation", data.Time, data.ExactPressure, "b-");
    plt::named_plot("Exact", data.Time, data.ExactPressure, "r--");
    plt::legend();
    plt::xlabel("Time [s]");
    plt::ylabel("Tank pressure [kPa]");
    plt::grid(true);
    plt::save("./plot/leak_tank_pressure.png");
}

int main()
{
    LeakNetwork network("LeakNetwork");
    network.initialize("LeakNetwork");

    ExactSolution exactSolution(
        network.netConfig.leak.mMaxConductivity / 2,         // leakArea: GUNNS conductivity must be divided by square root of two twice:
                                                             //           once because of the constant half in the Bernoulli equation,
                                                             //           and once because GUNNS averages the densities on the two sides of the link
        network.netInput.tank.mInitialVolume,                // tankVolume - Tank volume [m^3] (not used)
        network.tank.getNodeContent()->getPressure(),        // initialPressure - Initial tank pressure [kPa]
        network.tank.getNodeContent()->getTemperature(),     // initialTemperature - Initial tank temperature [K]
        network.tank.getNodeContent()->getMWeight() / 1000.0 // molarWeight - Molar weight of the gas [kg/mol]
    );

    int stepCount = 1000;
    double timeStep = 1;
    Data data;
    log(network, 0.0, data, exactSolution.getPressure(0.0));
    for (int step = 0; step < stepCount; ++step)
    {
        network.update(timeStep);

        double time = step * timeStep;
        log(network, time, data, exactSolution.getPressure(time));
    }

    plot(data);

    return 0;
}
