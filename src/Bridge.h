#ifndef Bridge_hpp
#define Bridge_hpp

#include <stdio.h>
#include <list>
#include "Camera.h"
#include "Model.h"
#include "Lights.h"

class Bridge : public BaseModel
{
public:
    Bridge();
    virtual ~Bridge();
    bool loadModel(const char* playerModel, int pieceId);
    virtual void draw(const BaseCamera& Cam);

protected:

    Model* BridgeModel;
    int pieceId;
    float speed;
    std::list<SpotLight> streetLights;

};

#endif /* Bridge_hpp */