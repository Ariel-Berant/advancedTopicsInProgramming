#include "../common/TankAlgorithmRegistration.h"
#include "AlgorithmRegistrar.h"
#include "../common/TankAlgorithm.h"

using namespace Simulator_0000;

TankAlgorithmRegistration::TankAlgorithmRegistration(TankAlgorithmFactory factory) {
    auto& registrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    registrar.addTankAlgorithmFactoryToLastEntry(std::move(factory));
}