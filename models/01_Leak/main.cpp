#include "gen/LeakNetwork.hh"
#include "matplotlib-cpp/matplotlibcpp.h"
#include "ExactSolution.hpp"

#include <iostream>

namespace plt = matplotlibcpp;

struct Data
{
    double Time;
    double SimulatedPressure;
    double ExactPressure;
};

void log(LeakNetwork const &network, double time, std::vector<Data> &data, double exactPressure)
{
    std::cout << "[" << time << "s] "
              << "Leak flowrate: " << network.leak.getFlowRate() << " kg/s | "
              << "Tank gas mass: " << network.tank.getNodeContent()->getMass() << " kg | "
              << "Tank pressure: " << network.tank.getNodeContent()->getPressure() << " kPa | "
              << "Tank O2 mole ratio: " << network.tank.getNodeContent()->getMoleFraction(FluidProperties::GUNNS_O2) * 100 << " % | " << std::endl;

    data.push_back({time, network.tank.getNodeContent()->getPressure(), exactPressure});
}

void plot(std::vector<Data> &data)
{
    std::vector<double> times(data.size());
    std::vector<double> simulatedPressures(data.size());
    std::vector<double> exactPressures(data.size());

    int index = 0;
    for (const auto &entry : data)
    {
        times[index] = entry.Time;
        simulatedPressures[index] = entry.SimulatedPressure;
        exactPressures[index] = entry.ExactPressure;
        index++;
    }

    plt::figure();
    plt::named_plot("Simulation", times, simulatedPressures, "b-");
    plt::named_plot("Exact", times, exactPressures, "r--");
    plt::legend();
    plt::xlabel("Time (s)");
    plt::ylabel("Tank Pressure (kPa)");
    plt::title("Leaking tank");
    plt::grid(true);
    plt::save("./plot/leak_tank_pressure.png");
}

int main(int argc, char **argv)
{

    LeakNetwork network("leak");
    network.initialize("leak");

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
    std::vector<Data> data;
    log(network, 0.0, data, exactSolution.getPressure(0.0));
    for (int step = 0; step < stepCount; ++step)
    {
        network.update(timeStep);

        double time = step * timeStep;
        log(network, time, data, exactSolution.getPressure(time));
    }

    plot(data);

    std::cout << network.tank.getNodeContent()->getMWeight() << std::endl;
    std::cout << network.tank.getNodeContent()->getTemperature() << std::endl;

    return 0;
}
