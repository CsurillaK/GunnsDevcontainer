#include "gen/LeakNetwork.hh"
#include "matplotlib-cpp/matplotlibcpp.h"

#include <iostream>

namespace plt = matplotlibcpp;

struct Data
{
    double Time;
    double Pressure;
};

void log(LeakNetwork const &network, double time, std::vector<Data> &data)
{
    std::cout << "[" << time << "s] "
              << "Leak flowrate: " << network.leak.getFlowRate() << " kg/s | "
              << "Tank gas mass: " << network.tank.getNodeContent()->getMass() << " kg | "
              << "Tank pressure: " << network.tank.getNodeContent()->getPressure() << " kPa | "
              << "Tank O2 mole ratio: " << network.tank.getNodeContent()->getMoleFraction(FluidProperties::GUNNS_O2) * 100 << " % | " << std::endl;

    data.push_back({time, network.tank.getNodeContent()->getPressure()});
}

void plot(std::vector<Data> &data)
{
    std::vector<double> times(data.size());
    std::vector<double> pressures(data.size());

    int index = 0;
    for (const auto &entry : data)
    {
        times[index] = entry.Time;
        pressures[index] = entry.Pressure;
        index++;
    }

    plt::figure();
    plt::plot(times, pressures);
    plt::xlabel("Time (s)");
    plt::ylabel("Tank Pressure (kPa)");
    plt::title("Leak Tank Pressure Over Time");
    plt::grid(true);
    plt::save("leak_tank_pressure.png");
}

int main(int argc, char **argv)
{

    LeakNetwork network("leak");
    network.initialize("leak");

    int stepCount = 1000;
    double timeStep = 1.0;
    std::vector<Data> data;
    log(network, 0.0, data);
    for (int step = 0; step < stepCount; ++step)
    {
        network.update(timeStep);
        log(network, step * timeStep, data);
    }

    plot(data);

    return 0;
}
