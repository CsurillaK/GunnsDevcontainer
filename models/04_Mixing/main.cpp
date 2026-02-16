#include "gen/MixingNetwork.hh"
#include "logging/GunnsLogger.hh"
#include "matplotlib-cpp/matplotlibcpp.h"

namespace plt = matplotlibcpp;

struct Data
{
    double Time;
    double MixingChamber1Carbonate; // Purity [-]
    double MixingChamber2Carbonate; // Purity [-]
    double MixingChamber2Acid;      // Purity [-]
    double SupplyPressure;          // [kPa]
    double MixingChamber1Pressure;  // [kPa]
    double MixingChamber2Pressure;  // [kPa]
    double Pipe1FlowRate;           // [kg/s]
    double Pipe2FlowRate;           // [kg/s]
    double DemandFlowRate;          // [kg/s]
};

void log(MixingNetwork const &network, double time, std::vector<Data> &data)
{
    data.push_back({time,
                    network.mixing_chamber_1.getNodeContent(0)->getTraceCompounds()->getMoleFraction(ChemicalCompound::LI2CO3),
                    network.mixing_chamber_2.getNodeContent(0)->getTraceCompounds()->getMoleFraction(ChemicalCompound::LI2CO3),
                    network.mixing_chamber_2.getNodeContent(0)->getTraceCompounds()->getMoleFraction(ChemicalCompound::H3PO4),
                    network.external_demand.getSourcePressure(),
                    network.mixing_chamber_1.getNodeContent(0)->getPressure(),
                    network.mixing_chamber_2.getNodeContent(0)->getPressure(),
                    network.pipe_1.getFlowRate(),
                    network.pipe_2.getFlowRate(),
                    network.external_supply.getFlowDemand()});
}

void plot(std::vector<Data> &data)
{
    std::vector<double> times(data.size());
    std::vector<double> mixingChamber1Carbonate(data.size());
    std::vector<double> mixingChamber2Carbonate(data.size());
    std::vector<double> mixingChamber2Acid(data.size());
    std::vector<double> supplyPressures(data.size());
    std::vector<double> mixingChamber1Pressures(data.size());
    std::vector<double> mixingChamber2Pressures(data.size());
    std::vector<double> pipe1FlowRates(data.size());
    std::vector<double> pipe2FlowRates(data.size());
    std::vector<double> demandFlowRates(data.size());

    int index = 0;
    for (const auto &entry : data)
    {
        times[index] = entry.Time;
        mixingChamber1Carbonate[index] = entry.MixingChamber1Carbonate;
        mixingChamber2Carbonate[index] = entry.MixingChamber2Carbonate;
        mixingChamber2Acid[index] = entry.MixingChamber2Acid;
        supplyPressures[index] = entry.SupplyPressure;
        mixingChamber1Pressures[index] = entry.MixingChamber1Pressure;
        mixingChamber2Pressures[index] = entry.MixingChamber2Pressure;
        pipe1FlowRates[index] = entry.Pipe1FlowRate;
        pipe2FlowRates[index] = entry.Pipe2FlowRate;
        demandFlowRates[index] = entry.DemandFlowRate;
        index++;
    }

    plt::figure();
    plt::named_plot("Chamber 1 - LI2CO3", times, mixingChamber1Carbonate, "r-");
    plt::named_plot("Chamber 2 - LI2CO3", times, mixingChamber2Carbonate, "g-");
    plt::named_plot("Chamber 2 - H3PO4", times, mixingChamber2Acid, "b-");
    plt::legend();
    plt::xlabel("Time [s]");
    plt::ylabel("Trace compound concentration [-]");
    plt::grid(true);
    plt::save("./plot/concentrations.png");

    plt::figure();
    plt::named_plot("Supply", times, supplyPressures, "r-");
    plt::named_plot("Chamber 1", times, mixingChamber1Pressures, "g-");
    plt::named_plot("Chamber 2", times, mixingChamber2Pressures, "b-");
    plt::legend();
    plt::xlabel("Time [s]");
    plt::ylabel("Pressure [kPa]");
    plt::grid(true);
    plt::save("./plot/pressures.png");

    plt::figure();
    plt::named_plot("Pipe 1", times, pipe1FlowRates, "r-");
    plt::named_plot("Pipe 2", times, pipe2FlowRates, "g-");
    plt::named_plot("Demand", times, demandFlowRates, "b-");
    plt::legend();
    plt::xlabel("Time [s]");
    plt::ylabel("Flow rate [kg/s]");
    plt::grid(true);
    plt::save("./plot/flowrates.png");
}

void simulateDuration(const double duration, double &time, const double timeStep, MixingNetwork &network, std::vector<Data> &data)
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

    MixingNetwork network("mixing");
    network.initialize("mixing");

    const double timestep = 0.1;
    double time = 0.0;
    std::vector<Data> data;
    log(network, time, data);

    network.external_supply.setFlowDemand(0.1);
    simulateDuration(1.0, time, timestep, network, data);

    network.lithium_carbonate_pump.setFlowDemand(0.01);
    simulateDuration(10.0, time, timestep, network, data);

    network.phosphoric_acid_pump.setFlowDemand(0.02);
    simulateDuration(10.0, time, timestep, network, data);

    network.lithium_carbonate_pump.setFlowDemand(0.0);
    network.phosphoric_acid_pump.setFlowDemand(0.0);
    simulateDuration(20.0, time, timestep, network, data);

    plot(data);

    return 0;
}
