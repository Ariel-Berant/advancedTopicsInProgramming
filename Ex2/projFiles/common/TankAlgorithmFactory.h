//
// Created by ariel on 5/8/2025.
//

#ifndef TANKALGORITHMFACTORY_H
#define TANKALGORITHMFACTORY_H
class TankAlgorithmFactory {
public:
    virtual ~TankAlgorithmFactory() {}
    virtual unique_ptr<TankAlgorithm> create(
            int player_index, int tank_index) const = 0;
};

#endif //ADVANCEDTOPICSINPROGRAMMING_TANKALGORITHMFACTORY_H
