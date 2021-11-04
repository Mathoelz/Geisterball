#ifndef Water_hpp
#define Water_hpp

#include <stdio.h>
#include <list>
#include "vector.h"
#include "Camera.h"
#include "Model.h"


class Water : public BaseModel
{
public:
    Water();
    virtual ~Water();
    bool loadModel(const char* waterModel, int pieceId);
    void update(float dtime);
    virtual void draw(const BaseCamera& Cam);
    void reset();

protected:
    Model* WaterModel;
    int pieceId;
    float speed;
};

#endif /* Water_hpp */
