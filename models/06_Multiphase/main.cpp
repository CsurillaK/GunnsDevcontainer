#include "gen/MultiphaseNetwork.hh"
#include "logging/GunnsLogger.hh"
#include "matplotlib-cpp/matplotlibcpp.h"

namespace plt = matplotlibcpp;

struct Data
{
    std::vector<double> Time;                // [s]
    std::vector<double> Tube_1_water_volume; // [ml]
    std::vector<double> Tube_2_water_volume; // [ml]
    // std::vector<double> Tube_1_pressure;     // [kPa]
    // std::vector<double> Tube_2_pressure;     // [kPa]
    std::vector<double> Tube_1_water_pressure; // [kPa]
    std::vector<double> Tube_1_air_pressure;   // [kPa]
    std::vector<double> Tube_2_air_pressure;   // [kPa]
    std::vector<double> Tube_2_water_pressure; // [kPa]
};

void log(MultiphaseNetwork const &network, double time, Data &data)
{
    data.Time.push_back(time);
    data.Tube_1_water_volume.push_back(network.tube_1.getBellowsPosition() * network.netConfig.tube_1.mAccumVolume * 1.0E6);
    data.Tube_2_water_volume.push_back(network.tube_2.getBellowsPosition() * network.netConfig.tube_2.mAccumVolume * 1.0E6);
    // data.Tube_1_pressure.push_back(network.tube_1.getNodeContent()->getPressure());
    // data.Tube_2_pressure.push_back(network.tube_2.getNodeContent()->getPressure());
    data.Tube_1_water_pressure.push_back(network.tube_1.getNodeContent(0)->getPressure());
    data.Tube_1_air_pressure.push_back(network.tube_1.getNodeContent(1)->getPressure());
    data.Tube_2_water_pressure.push_back(network.tube_2.getNodeContent(0)->getPressure());
    data.Tube_2_air_pressure.push_back(network.tube_2.getNodeContent()->getPressure());
}

void plot(Data const &data)
{
    plt::figure();
    plt::named_plot("Tube 1", data.Time, data.Tube_1_water_volume, "r-");
    plt::named_plot("Tube 2", data.Time, data.Tube_2_water_volume, "g-");
    plt::legend();
    plt::xlabel("Time [s]");
    plt::ylabel("Tube water volumne [ml]");
    plt::grid(true);
    plt::save("./plot/tube_water_volumes.png");

    plt::figure();
    plt::named_plot("Tube 1 water", data.Time, data.Tube_1_water_pressure, "r-");
    plt::named_plot("Tube 1 air", data.Time, data.Tube_1_air_pressure, "r--");
    plt::named_plot("Tube 2 water", data.Time, data.Tube_2_water_pressure, "g-");
    plt::named_plot("Tube 2 air", data.Time, data.Tube_2_air_pressure, "g--");
    plt::legend();
    plt::xlabel("Time [s]");
    plt::ylabel("Tube pressure [kPa]");
    plt::grid(true);
    plt::save("./plot/tube_pressures.png");
}

void updateTubeValves(GunnsFluidAccumGas const &tube, GunnsFluidValve &valve_air, GunnsFluidValve &valve_water)
{
    double bellowsPosition = tube.getBellowsPosition();
    if (bellowsPosition >= 0.90)
    {
        valve_air.setPosition(0.0);
        valve_water.setPosition(1.0);
    }
    else if (bellowsPosition <= 0.10)
    {
        valve_air.setPosition(1.0);
        valve_water.setPosition(0.0);
    }
}

void simulateDuration(const double duration, double &time, const double timeStep, MultiphaseNetwork &network, Data &data)
{
    const double endTime = time + duration;
    while (time < endTime)
    {
        time += timeStep;
        network.update(timeStep);
        log(network, time, data);

        updateTubeValves(network.tube_1, network.valve_1_air, network.valve_1_water);
        updateTubeValves(network.tube_2, network.valve_2_air, network.valve_2_water);
    }
}

int main()
{
    GunnsLogger logger;
    logger.init();

    MultiphaseNetwork network("multiphase");
    network.initialize("multiphase");

    const double timestep = 0.01;
    double time = 0.0;
    Data data;
    log(network, time, data);

    network.valve_cannister_air.setPosition(0.0);
    network.valve_cannister_water.setPosition(1.0);
    network.valve_1_air.setPosition(1.0);
    network.valve_1_water.setPosition(0.0);
    network.valve_2_air.setPosition(1.0);
    network.valve_2_water.setPosition(0.0);

    simulateDuration(10.0, time, timestep, network, data);

    network.valve_cannister_air.setPosition(1.0);
    network.valve_cannister_water.setPosition(0.0);

    simulateDuration(10.0, time, timestep, network, data);

    plot(data);

    return 0;
}
