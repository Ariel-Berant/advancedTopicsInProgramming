#ifndef UNMOVINGOBJECT_H
#define UNMOVINGOBJECT_H
#include "MatrixObject.h"

namespace UserCommon_0000 {

    class unmovingObject : public matrixObject
    {
        // The unmovingObject class inherits from matrixObject, allowing it to use the properties and methods of matrixObject.
        // This class represents an object that cannot move in a 2D space.
        
    protected:
    public:
        unmovingObject(int row, int col ,objectType oType);
        ~unmovingObject();
    };

}

#endif // UNMOVINGOBJECT_H