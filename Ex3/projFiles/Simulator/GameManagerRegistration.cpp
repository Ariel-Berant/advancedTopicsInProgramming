#include "../common/GameManagerRegistration.h"
#include "GameManagerRegistrar.h"
#include "../common/AbstractGameManager.h"

using namespace Simulator_0000;

GameManagerRegistration::GameManagerRegistration(GameManagerFactory factory) {
    auto& registrar = GameManagerRegistrar::getGameManagerRegistrar();
    registrar.addGameManagerFactoryToLastEntry(std::move(factory));
}