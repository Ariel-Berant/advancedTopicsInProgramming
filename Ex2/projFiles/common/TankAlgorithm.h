//
// Created by ariel on 5/8/2025.
//

#ifndef TANKALGORITHM_H
#define TANKALGORITHM_H
class TankAlgorithm {
public:
    virtual ~TankAlgorithm() {}
    virtual ActionRequest getAction() = 0;
    virtual void updateBattleInfo(BattleInfo& info) = 0;
};

#endif //ADVANCEDTOPICSINPROGRAMMING_TANKALGORITHM_H
