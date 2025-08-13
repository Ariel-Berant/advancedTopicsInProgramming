#pragma once

#include <cstddef>
#include "../common/SatelliteView.h"
#include "../UserCommon/MatrixObject.h"
#include "../UserCommon/ObjectType.h"

using std::shared_ptr;
using std::vector;
using std::array;

namespace UserCommon_0000 {
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
    OurSattelliteView(const vector<vector<array<shared_ptr<matrixObject>, 3>>> gBoard, size_t numOfCols, size_t numOfRows, size_t tankX, size_t tankY);
    ~OurSattelliteView() = default; // Default destructor
    OurSattelliteView(const OurSattelliteView&) = delete; // Disable copy constructor
    OurSattelliteView(OurSattelliteView&&) = default; // Enable move constructor
    char getObjectAt(size_t x, size_t y)const override;
};

}
