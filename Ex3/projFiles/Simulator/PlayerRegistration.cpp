#include "../common/PlayerRegistration.h"

#include "AlgorithmRegistrar.h"

using namespace Simulator_0000;

PlayerRegistration::PlayerRegistration(PlayerFactory factory) {
    auto& registrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    registrar.addPlayerFactoryToLastEntry(std::move(factory));
}