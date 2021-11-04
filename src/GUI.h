//
//  GUI.hpp
//  ogl4
//
//  Created by Philipp Lensing on 21.09.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#ifndef GUI_hpp
#define GUI_hpp

#include "Model.h"

class GUI : public BaseModel
{
public:
    GUI();
    virtual ~GUI();
    bool load();
    void update(float dtime);
    virtual void draw(const BaseCamera& Cam);

    void setStartMenuIsActive(bool status) { this->startMenuIsActive = status; };
    void setPauseMenuIsActive(bool status) { this->pauseMenuIsActive = status; };
    void setGameOverMenuIsActive(bool status) { this->gameOverMenuIsActive = status; };
protected:

    float scale;

    Model* startMenu;
    Model* pauseMenu;
    Model* gameOverMenu;

    bool startMenuIsActive = true;
    bool pauseMenuIsActive = false;
    bool gameOverMenuIsActive = false;
};

#endif /* GUI_hpp */
