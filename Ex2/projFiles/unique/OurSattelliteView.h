#ifndef OURSATTELLITEVIEW_H
#define OURSATTELLITEVIEW_H

#include <cstddef>
#include "../common/SattelliteView.h"
#include "gameManager.h"
#include "matrixObject.h"
#include "objectType.h"

class OurSatelliteView : public SatelliteView {
private:
    vector<vector<array<shared_ptr<matrixObject>, 3>>> gameBoard;
    int numOfRows;
    int numOfCols;
    size_t tankX;
    size_t tankY;
    // The gameBoard is a 2D vector of arrays, where each array contains three shared pointers to matrixObject.
    // The first element represents the wall, the second element represents the tank, and the third element represents the bullet
public:
    OurSatelliteView(vector<vector<array<shared_ptr<matrixObject>, 3>>> gBoard, int numOfRows, int numOfCols, size_t tankX, size_t tankY );
    ~OurSatelliteView() {}
    char getObjectAt(size_t x, size_t y)const override;
};

#endif //OURSATTELLITEVIEW_H
