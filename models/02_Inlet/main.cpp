#include "gen/InletNetwork.hh"
#include "logging/GunnsLogger.hh"
#include "matplotlib-cpp/matplotlibcpp.h"

namespace plt = matplotlibcpp;

struct Data
{
    std::vector<double> Time;          // [s]
    std::vector<double> Tube_Pressure; // [kPa]
    std::vector<double> Tube_Volume;   // [m^3]
};

void log(InletNetwork const &network, double time, Data &data)
{
    // std::cout << "[" << time << "s] "
    //           << "Gear pump flow rate: " << network.gear_pump.getFlowRate() << " kg/s | "
    //           << "Gear pump limiter flow rate: " << network.gear_pump_limiter.getFlowRate() << " kg/s | "
    //           << "Gear pump limiter position: " << network.gear_pump_limiter.getPosition() * 100 << " % | "
    //           << "Inlet tube bellows position: " << network.inlet_tube.getBellowsPosition() << std::endl;
    data.Time.push_back(time);
    data.Tube_Pressure.push_back(network.inlet_tube.getNodeContent(1)->getPressure()); // network.netNodes[InletNetwork::Nodes::Node1].getPotential()
    data.Tube_Volume.push_back(network.inlet_tube.getBellowsPosition() * network.netConfig.inlet_tube.mAccumVolume);
}

void plot(Data const &data)
{
    plt::figure();
    plt::plot(data.Time, data.Tube_Pressure);
    plt::xlabel("Time [s]");
    plt::ylabel("Tube pressure [kPa]");
    plt::grid(true);
    plt::save("./plot/inlet_tube_pressure.png");

    plt::figure();
    plt::plot(data.Time, data.Tube_Volume);
    plt::xlabel("Time [s]");
    plt::ylabel("Tube volume [ml]");
    plt::grid(true);
    plt::save("./plot/inlet_tube_volume.png");
}

void simulateDuration(const double duration, double &time, const double timeStep, InletNetwork &network, Data &data)
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
    Data data;
    log(network, time, data);

    simulateDuration(1.0, time, timestep, network, data);

    network.inlet_valve.setPosition(0.0);

    simulateDuration(5.0, time, timestep, network, data);

    network.inlet_valve.setPosition(1.0);

    simulateDuration(4.0, time, timestep, network, data);

    plot(data);

    return 0;
}
