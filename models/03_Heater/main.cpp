#include "gen/HeaterNetwork.hh"
#include "logging/GunnsLogger.hh"
#include "matplotlib-cpp/matplotlibcpp.h"

namespace plt = matplotlibcpp;

struct Data
{
    std::vector<double> Time;                     // [s]
    std::vector<double> Tank_heater_temperature;  // [°C]
    std::vector<double> Tube_in_temperature;      // [°C]
    std::vector<double> Tank_cooling_temperature; // [°C]
    std::vector<double> Tube_out_temperature;     // [°C]
    std::vector<double> System_mass;              // [kg]
    std::vector<double> Tank_heater_pressure;     // [kPa]
    std::vector<double> Tube_in_pressure;         // [kPa]
    std::vector<double> Tank_cooling_pressure;    // [kPa]
    std::vector<double> Tube_out_pressure;        // [kPa]
};

void log(HeaterNetwork const &network, double time, Data &data)
{
    data.Time.push_back(time);
    data.Tank_heater_temperature.push_back(network.tank_heater.getNodeContent(0)->getTemperature() - 273.15);
    data.Tube_in_temperature.push_back(network.tube_in.getNodeContent(0)->getTemperature() - 273.15);
    data.Tank_cooling_temperature.push_back(network.tank_cooling.getNodeContent(0)->getTemperature() - 273.15);
    data.Tube_out_temperature.push_back(network.tube_out.getNodeContent(0)->getTemperature() - 273.15);
    data.System_mass.push_back(network.tank_heater.getNodeContent(0)->getMass() + network.tube_in.getNodeContent(0)->getMass() + network.tank_cooling.getNodeContent(0)->getMass() + network.tube_out.getNodeContent(0)->getMass());
    data.Tank_heater_pressure.push_back(network.tank_heater.getNodeContent(0)->getPressure());
    data.Tube_in_pressure.push_back(network.tube_in.getNodeContent(0)->getPressure());
    data.Tank_cooling_pressure.push_back(network.tank_cooling.getNodeContent(0)->getPressure());
    data.Tube_out_pressure.push_back(network.tube_out.getNodeContent(0)->getPressure());
}

void plot(Data const &data)
{
    plt::figure();
    plt::named_plot("Tank Heater", data.Time, data.Tank_heater_temperature, "r-");
    plt::named_plot("Tube In", data.Time, data.Tube_in_temperature, "g-");
    plt::named_plot("Tank Cooling", data.Time, data.Tank_cooling_temperature, "b-");
    plt::named_plot("Tube Out", data.Time, data.Tube_out_temperature, "m-");
    plt::legend();
    plt::xlabel("Time [s]");
    plt::ylabel("Node temperatures [°C]");
    plt::grid(true);
    plt::save("./plot/node_temperatures.png");

    plt::figure();
    plt::plot(data.Time, data.System_mass);
    plt::xlabel("Time [s]");
    plt::ylabel("System mass [kg]");
    plt::grid(true);
    plt::save("./plot/node_system_mass.png");

    plt::figure();
    plt::named_plot("Tank Heater", data.Time, data.Tank_heater_pressure, "r-");
    plt::named_plot("Tube In", data.Time, data.Tube_in_pressure, "g-");
    plt::named_plot("Tank Cooling", data.Time, data.Tank_cooling_pressure, "b-");
    plt::named_plot("Tube Out", data.Time, data.Tube_out_pressure, "m-");
    plt::legend();
    plt::xlabel("Time [s]");
    plt::ylabel("Node pressures [kPa]");
    plt::grid(true);
    plt::save("./plot/node_pressures.png");
}

void simulateDuration(const double duration, double &time, const double timeStep, HeaterNetwork &network, Data &data)
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

    HeaterNetwork network("heater");
    network.initialize("heater");

    const double timestep = 0.1;
    double time = 0.0;
    Data data;
    log(network, time, data);
    simulateDuration(1.0, time, timestep, network, data);

    network.heater.setHeaterPower(1000.0);
    simulateDuration(20.0, time, timestep, network, data);

    network.centrifugal_pump.setMotorSpeed(5000.0);
    simulateDuration(200.0, time, timestep, network, data);

    network.heater.setHeaterPower(0.0);
    simulateDuration(300.0, time, timestep, network, data);

    plot(data);

    return 0;
}
