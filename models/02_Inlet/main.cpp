#include "gen/InletNetwork.hh"
#include "logging/GunnsLogger.hh"
#include "matplotlib-cpp/matplotlibcpp.h"

namespace plt = matplotlibcpp;

struct Data
{
    double Time;
    double Tube_Pressure; // kPa
    double Tube_Volume;   // m^3
};

void log(InletNetwork const &network, double time, std::vector<Data> &data)
{
    // std::cout << "[" << time << "s] "
    //           << "Gear pump flow rate: " << network.gear_pump.getFlowRate() << " kg/s | "
    //           << "Gear pump limiter flow rate: " << network.gear_pump_limiter.getFlowRate() << " kg/s | "
    //           << "Gear pump limiter position: " << network.gear_pump_limiter.getPosition() * 100 << " % | "
    //           << "Inlet tube bellows position: " << network.inlet_tube.getBellowsPosition() << std::endl;
    data.push_back({time,
                    network.inlet_tube.getNodeContent(1)->getPressure(), // network.netNodes[InletNetwork::Nodes::Node1].getPotential()
                    network.inlet_tube.getBellowsPosition() * network.netConfig.inlet_tube.mAccumVolume});
}

void plot(std::vector<Data> &data)
{
    std::vector<double> times(data.size());
    std::vector<double> tube_pressures(data.size());
    std::vector<double> tube_volumes(data.size());

    int index = 0;
    for (const auto &entry : data)
    {
        times[index] = entry.Time;
        tube_pressures[index] = entry.Tube_Pressure;
        tube_volumes[index] = entry.Tube_Volume * 1.0E6; // m^3 to ml
        index++;
    }

    plt::figure();
    plt::plot(times, tube_pressures);
    plt::xlabel("Time [s]");
    plt::ylabel("Tube pressure [kPa]");
    plt::grid(true);
    plt::save("./plot/inlet_tube_pressure.png");

    plt::figure();
    plt::plot(times, tube_volumes);
    plt::xlabel("Time [s]");
    plt::ylabel("Tube volume [ml]");
    plt::grid(true);
    plt::save("./plot/inlet_tube_volume.png");
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

    network.inlet_valve.setPosition(1.0);

    simulateDuration(4.0, time, timestep, network, data);

    plot(data);

    return 0;
}
