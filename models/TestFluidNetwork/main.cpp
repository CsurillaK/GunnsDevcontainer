#include <iostream>
#include "gen/TestFluidNetwork.hh"

void printSquareMatrix(const double* matrix, int dimension)
{
    std::cout << "Admittance Matrix:" << std::endl;
    for (int i = 0; i < dimension; ++i) {
        for (int j = 0; j < dimension; ++j) {
            std::cout << matrix[i * dimension + j] << "\t";
        }
        std::cout << std::endl;
    }
}

int main(int argc, char** argv) {

    TestFluidNetwork network("fluid");
    network.initialize("fluid");

    for (int step=0; step<30; ++step) {
        network.update(1);
        std::cout << "Leak flowrate: " << network.leak.getFlowRate() << " kg/s | " 
                  << "Tank gas mass: " << network.tank.getNodeContent()->getMass() << " kg" << std::endl;
    }

    printSquareMatrix(network.netSolver.getAdmittanceMatrix(), network.N_NODES - 1);

    return 0;
}
