//
//  Application.hpp
//  ogl4
//
//  Created by Philipp Lensing on 16.09.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#ifndef Application_hpp
#define Application_hpp

#include <irrKlang.h>
#include <stdio.h>
#include <list>
#include "camera.h"
#include "phongshader.h"
#include "constantshader.h"
#include "vertexbuffer.h"
#include "indexbuffer.h"
#include "basemodel.h"
#include "ShadowMapGenerator.h"
#include "Bridge.h"
#include "Character.h"
#include "GUI.h"
#include "Obstacle.h"
#include "Water.h"

class Application
{
public:
    typedef std::list<BaseModel*> ModelList;
    Application(GLFWwindow* pWin);
    void start();
    void update(float dtime);
    void draw();
    void end();
protected:
    void createStartMenu();
	void createScene();
    void createCharacter();
    void createBridge();
    void createSkyBox();
    void createObstacles();
    void updateObstacles(float dtime);
    void updateWaterElements(float dtime);
    void controlCharacter();
    void createWater();

    irrklang::ISoundEngine* SoundEngine;
    Camera Cam;
    ModelList Models;
    GLFWwindow* pWindow;

    GUI* gui;

	BaseModel* pModel;
	ShadowMapGenerator ShadowGenerator;

    // Character
    Character* PlayerCharacter;

    Model* testModel;
    // Bridge
    Bridge* bridgePiece;
    // Obstacles
    std::list<Obstacle*> Obstacles;
    Obstacle* obstacle;
    // Water
    std::list<Water*> WaterElements;
    Water* water;

    // Additional obstacle Variables
    float difficultyChangeInterval;
    float difficultyIncreaseAmount;
    int maxDifficultyChanges;
    int currentDifficulty;

    bool gameIsRunning = false;
    bool gameIsStarted = false;
    bool gameIsOver = false;
};

#endif /* Application_hpp */
