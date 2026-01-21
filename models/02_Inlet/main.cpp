#include "gen/InletNetwork.hh"
#include "logging/GunnsLogger.hh"
#include "matplotlib-cpp/matplotlibcpp.h"

namespace plt = matplotlibcpp;

struct Data
{
    double Time;
    double Pressure;
};

void log(InletNetwork const &network, double time, std::vector<Data> &data)
{
    std::cout << "[" << time << "s] "
              << "Gear pump flow rate: " << network.gear_pump.getFlowRate() << " kg/s | "
              << "Inlet valve flow rate: " << network.inlet_valve.getFlowRate() << " kg/s | " 
              << "Ambient tank bellows position:" << network.ambient_tank.getBellowsPosition() << std::endl;
    // data.push_back({time, network.netNodes[InletNetwork::Nodes::Node1].getPotential()});
    data.push_back({time, network.inlet_tube.getNodeContent(1)->getPressure()});
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
    plt::ylabel("Tube pressure (kPa)");
    plt::grid(true);
    plt::save("./plot/inlet_tube_pressure.png");
}

void simulateDuration(const double duration, double &time, const double timeStep, InletNetwork &network, std::vector<Data> &data)
{
    const double endTime = time + duration;
    while (time < endTime)
    {
        time += timeStep;
        network.update(timeStep);
        log(network, time, data);
    }
}

int main()
{
    GunnsLogger logger;
    logger.init();

    InletNetwork network("inlet");
    network.initialize("inlet");

    const double timestep = 0.1;
    double time = 0.0;
    std::vector<Data> data;
    log(network, time, data);

    simulateDuration(1.0, time, timestep, network, data);

    network.inlet_valve.setPosition(0.0);

    simulateDuration(5.0, time, timestep, network, data);

    plot(data);

    return 0;
}
