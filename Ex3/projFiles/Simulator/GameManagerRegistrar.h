#pragma once

#include <string>
#include <memory>
#include <vector>
#include <cassert>

#include "../common/AbstractGameManager.h"
#include "GameManagerRegistration.h"

namespace Simulator_0000
{
    class GameManagerRegistrar
    {
        class GameManagerWrapper{
            std::string so_name;
            GameManagerFactory gameManagerFactory;
        public:
            GameManagerWrapper(const std::string& so_name) : so_name(so_name) {}
            
            void setGameManagerFactory(GameManagerFactory&& factory) {
                assert(gameManagerFactory == nullptr);
                gameManagerFactory = std::move(factory);
            }

            const std::string& name() const { return so_name; }
            
            std::unique_ptr<AbstractGameManager> createGameManager(bool verbose) const {
                return gameManagerFactory(verbose);
            }
            
            bool hasGameManagerFactory() const {
                return gameManagerFactory != nullptr;
            }
        };

        
        std::vector<GameManagerWrapper> gameManagers;
        static GameManagerRegistrar registrar;
    public:
        static GameManagerRegistrar& getGameManagerRegistrar();
        
        void createGameManagerFactoryEntry(const std::string& name) {
            gameManagers.emplace_back(name);
        }

        void addGameManagerFactoryToLastEntry(GameManagerFactory&& factory) {
            gameManagers.back().setGameManagerFactory(std::move(factory));
        }

        struct BadRegistrationException {
            std::string name;
            bool hasName, hasGameManagerFactory;
        };

        void validateLastRegistration() {
            const auto& last = gameManagers.back();
            bool hasName = (last.name() != "");
            bool hasGameManagerFactory = last.hasGameManagerFactory();

            if (!hasName || !hasGameManagerFactory) {
                throw BadRegistrationException{ 
                    .name = last.name(), 
                    .hasName = hasName, 
                    .hasGameManagerFactory = hasGameManagerFactory 
                };
            }
        }
        
        void removeLast() {
            gameManagers.pop_back();
        }
        
        auto begin() const {
            return gameManagers.begin();
        }
        
        auto end() const {
            return gameManagers.end();
        }

        std::size_t count() const { return gameManagers.size(); }

        void clear() {
            gameManagers.clear();
        }
    };
} 
