#include "gen/MixingNetwork.hh"
#include "logging/GunnsLogger.hh"
#include "matplotlib-cpp/matplotlibcpp.h"

namespace plt = matplotlibcpp;

struct Data
{
    std::vector<double> Time;                    // [s]
    std::vector<double> MixingChamber1Carbonate; // Purity [%]
    std::vector<double> MixingChamber2Carbonate; // Purity [%]
    std::vector<double> MixingChamber2Acid;      // Purity [%]
    std::vector<double> SupplyPressure;          // [kPa]
    std::vector<double> MixingChamber1Pressure;  // [kPa]
    std::vector<double> MixingChamber2Pressure;  // [kPa]
    std::vector<double> Pipe1FlowRate;           // [kg/s]
    std::vector<double> Pipe2FlowRate;           // [kg/s]
    std::vector<double> DemandFlowRate;          // [kg/s]
};

// @return Mass percentage, relative to reference solution [%]
double calculateConcentration(PolyFluid const *nodeContent, ChemicalCompound::Type compound)
{
    return nodeContent->getTraceCompounds()->getMass(compound) / nodeContent->getMass() * 100;
}

void log(MixingNetwork const &network, double time, Data &data)
{
    data.Time.push_back(time);
    data.MixingChamber1Carbonate.push_back(calculateConcentration(network.mixing_chamber_1.getNodeContent(0), ChemicalCompound::LI2CO3));
    data.MixingChamber2Carbonate.push_back(calculateConcentration(network.mixing_chamber_2.getNodeContent(0), ChemicalCompound::LI2CO3));
    data.MixingChamber2Acid.push_back(calculateConcentration(network.mixing_chamber_2.getNodeContent(0), ChemicalCompound::H3PO4));
    data.SupplyPressure.push_back(network.external_demand.getSourcePressure());
    data.MixingChamber1Pressure.push_back(network.mixing_chamber_1.getNodeContent(0)->getPressure());
    data.MixingChamber2Pressure.push_back(network.mixing_chamber_2.getNodeContent(0)->getPressure());
    data.Pipe1FlowRate.push_back(network.pipe_1.getFlowRate());
    data.Pipe2FlowRate.push_back(network.pipe_2.getFlowRate());
    data.DemandFlowRate.push_back(network.external_supply.getFlowDemand());
}

void plot(Data const &data)
{
    plt::figure();
    plt::named_plot("Chamber 1 - LI2CO3", data.Time, data.MixingChamber1Carbonate, "r-");
    plt::named_plot("Chamber 2 - LI2CO3", data.Time, data.MixingChamber2Carbonate, "g-");
    plt::named_plot("Chamber 2 - H3PO4", data.Time, data.MixingChamber2Acid, "b-");
    plt::legend();
    plt::xlabel("Time [s]");
    plt::ylabel("Mass concentration [%]");
    plt::grid(true);
    plt::save("./plot/concentrations.png");

    plt::figure();
    plt::named_plot("Supply", data.Time, data.SupplyPressure, "r-");
    plt::named_plot("Chamber 1", data.Time, data.MixingChamber1Pressure, "g-");
    plt::named_plot("Chamber 2", data.Time, data.MixingChamber2Pressure, "b-");
    plt::legend();
    plt::xlabel("Time [s]");
    plt::ylabel("Pressure [kPa]");
    plt::grid(true);
    plt::save("./plot/pressures.png");

    plt::figure();
    plt::named_plot("Pipe 1", data.Time, data.Pipe1FlowRate, "r-");
    plt::named_plot("Pipe 2", data.Time, data.Pipe2FlowRate, "g-");
    plt::named_plot("Demand", data.Time, data.DemandFlowRate, "b-");
    plt::legend();
    plt::xlabel("Time [s]");
    plt::ylabel("Flow rate [kg/s]");
    plt::grid(true);
    plt::save("./plot/flowrates.png");
}

void simulateDuration(const double duration, double &time, const double timeStep, MixingNetwork &network, Data &data)
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
    Data data;
    log(network, time, data);

    network.external_supply.setFlowDemand(0.1);
    simulateDuration(1.0, time, timestep, network, data);

    network.lithium_carbonate_pump.setFlowDemand(0.01);
    simulateDuration(40.0, time, timestep, network, data);

    network.phosphoric_acid_pump.setFlowDemand(0.02);
    simulateDuration(40.0, time, timestep, network, data);

    network.lithium_carbonate_pump.setFlowDemand(0.0);
    network.phosphoric_acid_pump.setFlowDemand(0.0);
    simulateDuration(40.0, time, timestep, network, data);

    plot(data);

    return 0;
}
