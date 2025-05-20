#ifndef PLAYERBATTLEINFO_H
#define PLAYERBATTLEINFO_H

#include "../common/BattleInfo.h"


class PlayerBattleInfo : BattleInfo{
private:
    vector<vector<array<shared_ptr<matrixObject>, 3>>> tankGameBoard;
public:
    PlayerBattleInfo(vector<vector<array<shared_ptr<matrixObject>, 3>>> &tankGameBoard): tankGameBoard(tankGameBoard) {
    }
    ~PlayerBattleInfo();
};

#endif// PlayerBattleInfo.
