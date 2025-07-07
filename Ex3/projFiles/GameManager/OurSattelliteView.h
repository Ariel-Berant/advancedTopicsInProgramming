#ifndef OURSATTELLITEVIEW_H
#define OURSATTELLITEVIEW_H

#include <cstddef>
#include "../common/SattelliteView.h"
#include "../UserCommon/MatrixObject.h"
#include "../UserCommon/ObjectType.h"
#include "GameManager.h"

class OurSattelliteView : public SatelliteView {
private:
    vector<vector<array<shared_ptr<matrixObject>, 3>>> gameBoard;
    size_t numOfRows;
    size_t numOfCols;
    size_t tankX;
    size_t tankY;
    // The gameBoard is a 2D vector of arrays, where each array contains three shared pointers to matrixObject.
    // The first element represents the wall, the second element represents the tank, and the third element represents the bullet
public:
    OurSattelliteView(const vector<vector<array<shared_ptr<matrixObject>, 3>>> gBoard, int numOfCols, int numOfRows, size_t tankX, size_t tankY);
    ~OurSattelliteView() = default; // Default destructor
    OurSattelliteView(const OurSattelliteView&) = delete; // Disable copy constructor
    char getObjectAt(size_t x, size_t y)const override;
};

#endif //OURSATTELLITEVIEW_H
