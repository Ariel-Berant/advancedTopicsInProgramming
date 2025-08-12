# Default target to build all projects
all: simulator algorithm game_manager

# Target to build the Simulator executable
simulator:
	$(MAKE) -C Simulator

# Target to build the Algorithm shared library
algorithm:
	$(MAKE) -C Algorithm

# Target to build the GameManager shared library
game_manager:
	$(MAKE) -C GameManager

# Target to clean all projects
clean:
	$(MAKE) -C Simulator clean
	$(MAKE) -C Algorithm clean
	$(MAKE) -C GameManager clean
	rm -f GameManager/GameManager_0000.so Algorithm/Algorithm_0000.so Simulator/simulator_0000

# Phony targets
.PHONY: all simulator algorithm game_manager clean