#include "gen/HeaterNetwork.hh"
#include "logging/GunnsLogger.hh"
#include "matplotlib-cpp/matplotlibcpp.h"

namespace plt = matplotlibcpp;

struct Data
{
    double Time;
    double Tank_heater_temperature;  // K
    double Tube_in_temperature;      // K
    double Tank_cooling_temperature; // K
    double Tube_out_temperature;     // K
    double System_mass;              // kg
    double Tank_heater_pressure;     // kPa
    double Tube_in_pressure;         // kPa
    double Tank_cooling_pressure;    // kPa
    double Tube_out_pressure;        // kPa
};

void log(HeaterNetwork const &network, double time, std::vector<Data> &data)
{
    data.push_back({time,
                    network.tank_heater.getNodeContent(0)->getTemperature(),
                    network.tube_in.getNodeContent(0)->getTemperature(),
                    network.tank_cooling.getNodeContent(0)->getTemperature(),
                    network.tube_out.getNodeContent(0)->getTemperature(),
                    network.tank_heater.getNodeContent(0)->getMass() + network.tube_in.getNodeContent(0)->getMass() + network.tank_cooling.getNodeContent(0)->getMass() + network.tube_out.getNodeContent(0)->getMass(),
                    network.tank_heater.getNodeContent(0)->getPressure(),
                    network.tube_in.getNodeContent(0)->getPressure(),
                    network.tank_cooling.getNodeContent(0)->getPressure(),
                    network.tube_out.getNodeContent(0)->getPressure()});
}

void plot(std::vector<Data> &data)
{
    std::vector<double> times(data.size());
    std::vector<double> tank_heater_temperatures(data.size());
    std::vector<double> tube_in_temperatures(data.size());
    std::vector<double> tank_cooling_temperatures(data.size());
    std::vector<double> tube_out_temperatures(data.size());
    std::vector<double> system_masses(data.size());
    std::vector<double> tank_heater_pressures(data.size());
    std::vector<double> tube_in_pressures(data.size());
    std::vector<double> tank_cooling_pressures(data.size());
    std::vector<double> tube_out_pressures(data.size());

    int index = 0;
    for (const auto &entry : data)
    {
        times[index] = entry.Time;
        tank_heater_temperatures[index] = entry.Tank_heater_temperature - 273.15;
        tube_in_temperatures[index] = entry.Tube_in_temperature - 273.15;
        tank_cooling_temperatures[index] = entry.Tank_cooling_temperature - 273.15;
        tube_out_temperatures[index] = entry.Tube_out_temperature - 273.15;
        system_masses[index] = entry.System_mass;
        tank_heater_pressures[index] = entry.Tank_heater_pressure;
        tube_in_pressures[index] = entry.Tube_in_pressure;
        tank_cooling_pressures[index] = entry.Tank_cooling_pressure;
        tube_out_pressures[index] = entry.Tube_out_pressure;
        index++;
    }

    plt::figure();
    plt::named_plot("Tank Heater", times, tank_heater_temperatures, "r-");
    plt::named_plot("Tube In", times, tube_in_temperatures, "g-");
    plt::named_plot("Tank Cooling", times, tank_cooling_temperatures, "b-");
    plt::named_plot("Tube Out", times, tube_out_temperatures, "m-");
    plt::legend();
    plt::xlabel("Time [s]");
    plt::ylabel("Node temperatures [°C]");
    plt::grid(true);
    plt::save("./plot/node_temperatures.png");

    plt::figure();
    plt::plot(times, system_masses);
    plt::xlabel("Time [s]");
    plt::ylabel("System mass [kg]");
    plt::grid(true);
    plt::save("./plot/node_system_mass.png");

    plt::figure();
    plt::named_plot("Tank Heater", times, tank_heater_pressures, "r-");
    plt::named_plot("Tube In", times, tube_in_pressures, "g-");
    plt::named_plot("Tank Cooling", times, tank_cooling_pressures, "b-");
    plt::named_plot("Tube Out", times, tube_out_pressures, "m-");
    plt::legend();
    plt::xlabel("Time [s]");
    plt::ylabel("Node pressures [kPa]");
    plt::grid(true);
    plt::save("./plot/node_pressures.png");
}

void simulateDuration(const double duration, double &time, const double timeStep, HeaterNetwork &network, std::vector<Data> &data)
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
    std::vector<Data> data;
    log(network, time, data);
    simulateDuration(1.0, time, timestep, network, data);

    network.heater.setHeaterPower(1000.0);
    simulateDuration(20.0, time, timestep, network, data);

    network.centrifugal_pump.setMotorSpeed(5000.0);
    simulateDuration(200.0, time, timestep, network, data);

    network.heater.setHeaterPower(0.0);
    simulateDuration(200.0, time, timestep, network, data);

    plot(data);

    return 0;
}
