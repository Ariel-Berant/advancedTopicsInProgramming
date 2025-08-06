#include "AlgorithmRegistrar.h"

using namespace Simulator_0000;

AlgorithmRegistrar AlgorithmRegistrar::registrar;

AlgorithmRegistrar& AlgorithmRegistrar::getAlgorithmRegistrar() {
    return registrar;
}