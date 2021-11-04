//
//  Character.hpp
//  CGXcode
//
//  Created by Philipp Lensing on 16.11.16.
//  Copyright 2016 Philipp Lensing. All rights reserved.
//

#ifndef Character_hpp
#define Character_hpp

#include <stdio.h>
#include <list>
#include "vector.h"
#include "Camera.h"
#include "Model.h"


class Character : public BaseModel
{
public:
    Character();
    virtual ~Character();
    bool loadModel(const char* playerModel);
    void update(float dtime);
    void steer(float LeftRight) { this->LeftRight = LeftRight; };
    virtual void draw(const BaseCamera& Cam) { this->PlayerCharacter->draw(Cam); };

    /* -- My Functions -- */
    void gameReset();
    bool checkCollision(const Model* model);
    Vector getCurrentPosition() { return this->position; };
    void setCurrentPosition(Vector position) { this->position = position; };

protected:
    Model* PlayerCharacter;

    float LeftRight = 0;
    float characterAngle;
    Vector drivenWay;
    Vector position;
};

#endif /* Character_hpp */
