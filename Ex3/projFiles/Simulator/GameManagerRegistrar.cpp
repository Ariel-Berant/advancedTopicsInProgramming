#include "GameManagerRegistrar.h"

using namespace Simulator_0000;

GameManagerRegistrar GameManagerRegistrar::registrar;

GameManagerRegistrar& GameManagerRegistrar::getGameManagerRegistrar() {
    return registrar;
}