//
//  Application.cpp
//  ogl4
//
//  Created by Philipp Lensing on 16.09.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#include "Application.h"
#ifdef WIN32
#include <GL/glew.h>
#include <glfw/glfw3.h>
#define _USE_MATH_DEFINES
#include <math.h>
#else
#define GLFW_INCLUDE_GLCOREARB
#define GLFW_INCLUDE_GLEXT
#include <glfw/glfw3.h>
#endif
#include "model.h"
#include "ShaderLightmapper.h"
#include <iostream>
using namespace std;


#ifdef WIN32
#define ASSET_DIRECTORY "../../assets/"
#else
#define ASSET_DIRECTORY "../assets/"
#endif

/*
 Erzeugt die Application und erstellt alle benötigten Ressourcen.
*/
Application::Application(GLFWwindow* pWin) : pWindow(pWin), Cam(pWin), pModel(NULL), ShadowGenerator(2048, 2048)
{
	this->SoundEngine = irrklang::createIrrKlangDevice();
	this->SoundEngine->play2D(ASSET_DIRECTORY "Audio/mainsound.mp3", true);


	this->createScene();

	this->createStartMenu();
	this->createCharacter();
	this->createBridge();
	this->createWater();
	this->createSkyBox();
	this->createObstacles();
}

/*
 Start-Methode.
*/
void Application::start()
{

    glEnable (GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc (GL_LESS); // depth-testing interprets a smaller value as "closer"
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	Cam.update();

}

/*
 Update Methode, stellt die Game-Loop dar.
*/
void Application::update(float dtime)
{
	this->controlCharacter();

	// Wenn das Speil noch nicht loeft und SPACE gedrueckt wurde 
	if (!gameIsStarted && glfwGetKey(pWindow, GLFW_KEY_SPACE) == GLFW_PRESS) {
		gui->setStartMenuIsActive(false);
		this->gameIsStarted = true;
		this->gameIsRunning = true;
	}
	// Wenn das Speil noch nicht loeft und ESC gedrueckt wurde 
	if (!gameIsStarted && glfwGetKey(pWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(pWindow, true);
	}
	// Wenn das Speil loeft und ESC gedrueckt wurde 
	if (gameIsStarted && !gameIsOver && glfwGetKey(pWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		gui->setPauseMenuIsActive(true);
		this->gameIsRunning = false;
	}

	// Wenn Pausen-Menu angezeigt wurd und Taste R gedrueckt wird
	if (gameIsStarted && !gameIsRunning && glfwGetKey(pWindow, GLFW_KEY_R) == GLFW_PRESS) {
		gui->setPauseMenuIsActive(false);
		this->gameIsRunning = true;
	}

	// Wenn Pausen-Menu angezeigt wurd und Taste ESC gedrueckt wird
	if (gameIsStarted && !gameIsRunning && glfwGetKey(pWindow, GLFW_KEY_E) == GLFW_PRESS) {
		glfwSetWindowShouldClose(pWindow, true);
	}

	// Wenn GameOver-Menu angezeigt wurd und Taste R gedrueckt wird
	if (gameIsStarted && gameIsOver && glfwGetKey(pWindow, GLFW_KEY_R) == GLFW_PRESS) {
		gui->setGameOverMenuIsActive(false);
		this->gameIsRunning = true;
		this->gameIsOver = false;
		// hier das Spiel neustarten
	}

	// Wenn GameOver-Menu angezeigt wurd und Taste ESC gedrueckt wird
	if (!gameIsRunning && gameIsOver && glfwGetKey(pWindow, GLFW_KEY_E) == GLFW_PRESS) {
		glfwSetWindowShouldClose(pWindow, true);
	}

	// check collision
	for (Obstacle* o : this->Obstacles) {
		if (PlayerCharacter->checkCollision(o->getModel()) == true) {
			cout << "\nKollision mit Hindernis!";
			this->PlayerCharacter->gameReset();
			for (Obstacle* o : this->Obstacles) {
				o->gameReset();
			}
			Cam.setPosition(Vector(0.0f, 10.0f, 27.5f));
			this->SoundEngine->play2D(ASSET_DIRECTORY "Audio/gameover_1.mp3", false);
			gui->setGameOverMenuIsActive(true);
			this->gameIsRunning = false;
			this->gameIsOver = true;
		}
	}

	if (gameIsRunning == true) {
		Cam.update();
		PlayerCharacter->update(dtime);
		this->updateObstacles(dtime);
		this->updateWaterElements(dtime);
	}
}

/*
 Methode zum darstellen aller Modelle.
*/
void Application::draw()
{
	ShadowGenerator.generate(Models);
	
    // 1. clear screen
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ShaderLightMapper::instance().activate();
    // 2. setup shaders and draw models
    for( ModelList::iterator it = Models.begin(); it != Models.end(); ++it )
    {
        (*it)->draw(Cam);
    }

	gui->draw(Cam);


	ShaderLightMapper::instance().deactivate();
	
    // 3. check once per frame for opengl errors
    GLenum Error = glGetError();
    assert(Error==0);
}

/*
 Beendet die Applikation und löscht alle erzeugten Modelle aus dem Speicher.
*/
void Application::end()
{
	/*for (ModelList::iterator it = Models.begin(); it != Models.end(); ++it)
	{
		delete* it;
	}
	*/
    this->Models.clear();
	this->Obstacles.clear();
	this->WaterElements.clear();
	this->SoundEngine->drop();
}

/*
 Erzeugt das Graphical User Interface.
*/
void Application::createStartMenu() {
	/* HelpMenu - Ein-/Ausblenden mit ESC */
	gui = new GUI();
	
	//Models.push_back(startmenu);
}

/*
 Methode zum Erzeugen des Playercharacters.
*/
void Application::createCharacter()
{
	PlayerCharacter = new Character();

	PlayerCharacter->shader(new PhongShader(), true);
	PlayerCharacter->loadModel(ASSET_DIRECTORY "Models/PlayerCharacter.obj");

	Models.push_back(PlayerCharacter);

}

/*
 Methode zum erzeugen der Brückenelemente, welche die komplette Brücke bilden werden.
*/
void Application::createBridge() {
	for (int i = 0; i < 2; ++i) {
		this->bridgePiece = new Bridge();
		bridgePiece->shader(new PhongShader(), true);
		bridgePiece->loadModel(ASSET_DIRECTORY "Models/bridge/bridge.obj", i);
		Models.push_back(bridgePiece);
	}
	this->bridgePiece = NULL;
}

/*
 Erzeugt die Skybox.
*/
void Application::createSkyBox()
{
	pModel = new Model(ASSET_DIRECTORY "Models/skybox/skybox.obj", false);
	pModel->shader(new PhongShader(), true);
	Models.push_back(pModel);
}

/*
 Erzeugt die Fahrzeuge welchen ausgewichen werden soll.
*/
void Application::createObstacles()
{	
	std::vector<const char*> cars = {
		ASSET_DIRECTORY "Models/cars/car_1.dae" ,
		ASSET_DIRECTORY "Models/cars/car_2.dae" ,
		ASSET_DIRECTORY "Models/cars/car_3.dae" ,
		ASSET_DIRECTORY "Models/cars/car_police.dae",
		ASSET_DIRECTORY "Models/cars/car_1.dae" ,
		ASSET_DIRECTORY "Models/cars/car_2.dae" ,
	};

	for (int i = 0; i < cars.size(); i++) {

		this->obstacle = new Obstacle();
		this->obstacle->shader(new PhongShader(), true);
		this->obstacle->loadModels(cars[i], i);

		this->obstacle->shadowCaster(true);
		this->Obstacles.push_back(this->obstacle);
		this->Models.push_back(this->obstacle);

	}
}

/*
 Schleife welche jedes einzelne Hindernis updated.
*/
void Application::updateObstacles(float dtime)
{
	// update all Obstacles
	for (auto const& i : this->Obstacles)
	{
		i->update(dtime);
	}
}

/*
 Methode zum Steuern des Charakters, dabei wird ebenfalls bei Bewegung
 die Kamera mitbewegt.
*/
void Application::controlCharacter()
{
	// Rechts 
	if (glfwGetKey(pWindow, GLFW_KEY_D) == GLFW_PRESS) {
		// Rechts + Links
		if (glfwGetKey(pWindow, GLFW_KEY_A) == GLFW_PRESS) {
			this->PlayerCharacter->steer(0.0f);
		}
		else {
			// Solange die Position X des Spielers kleiner als 2.9f ist
			if (this->PlayerCharacter->getCurrentPosition().X >= 2.9f) {}
			else {
				this->PlayerCharacter->steer(1.0f);
				//Cam.rotate(10.f, 10.f);
				Cam.pan(0.01f, 0.f);
			}
		}
	}
	// Links
	if (glfwGetKey(pWindow, GLFW_KEY_A) == GLFW_PRESS) {
		// Links + Rechts
		if (glfwGetKey(pWindow, GLFW_KEY_D) == GLFW_PRESS) {
			this->PlayerCharacter->steer(0.0f);
		}
		else {
			// Solange die Position X des Spielers größer als -2.05f ist
			if (this->PlayerCharacter->getCurrentPosition().X <= -2.05f) {}
			else {
				this->PlayerCharacter->steer(-1.0f);
				//Cam.rotate(-10.f, -10.f);
				Cam.pan(-0.01f, 0.f);
			}
		}
	}
}

/*
 Erzeugt die gesamten in der Szene benötigten Lichter.
*/
void Application::createScene()
{
	Matrix m, n;

	// sunset color
	Color sunsetColor = Color(1.0f, 0.45f, 0.25f);
	Vector a0 = Vector(0.001, 0, 0.0001f);

	// sun point light
	PointLight* pl = new PointLight();
	pl = new PointLight();
	pl->position(Vector(50.f, 50.f, -65));
	pl->color(sunsetColor);
	pl->attenuation(a0);
	ShaderLightMapper::instance().addLight(pl);


	Color c = Color(1.0f, 0.7f, 1.0f);
	float innerradius = 15.f;
	float outerradius = 30.f;

	// spot lights
	// 1-Right 
	SpotLight* sl = new SpotLight();
	sl->position(Vector(2.75f, 15.f, 25.0f));
	sl->color(c);
	sl->direction(Vector(-1, -5, 0));
	sl->innerRadius(innerradius);
	sl->outerRadius(outerradius);
	ShaderLightMapper::instance().addLight(sl);
	// 2-Right 
	sl = new SpotLight();
	sl->position(Vector(2.75f, 15.f, 15.4f));
	sl->color(c);
	sl->direction(Vector(-1, -5, 0));
	sl->innerRadius(innerradius);
	sl->outerRadius(outerradius);
	ShaderLightMapper::instance().addLight(sl);
	// 3-Right 
	sl = new SpotLight();
	sl->position(Vector(2.75f, 15.f, 5.4f));
	sl->color(c);
	sl->direction(Vector(-1, -5, 0));
	sl->innerRadius(innerradius);
	sl->outerRadius(outerradius);
	ShaderLightMapper::instance().addLight(sl);
	// 4-Right 
	sl = new SpotLight();
	sl->position(Vector(2.75f, 15.f, -4.1f));
	sl->color(c);
	sl->direction(Vector(-1, -5, 0));
	sl->innerRadius(innerradius);
	sl->outerRadius(outerradius);
	ShaderLightMapper::instance().addLight(sl);
	// 5-Right 
	sl = new SpotLight();
	sl->position(Vector(2.75f, 15.f, -14.1f));
	sl->color(c);
	sl->direction(Vector(-1, -5, 0));
	sl->innerRadius(innerradius);
	sl->outerRadius(outerradius);
	ShaderLightMapper::instance().addLight(sl);
	// 6-Right 
	sl = new SpotLight();
	sl->position(Vector(2.75f, 15.f, -23.6f));
	sl->color(c);
	sl->direction(Vector(-1, -5, 0));
	sl->innerRadius(innerradius);
	sl->outerRadius(outerradius);
	ShaderLightMapper::instance().addLight(sl);

	// 1-Left 
	sl = new SpotLight();
	sl->position(Vector(-3.5f, 15.f, 25.0f));
	sl->color(c);
	sl->direction(Vector(1, -5, 0));
	sl->innerRadius(innerradius);
	sl->outerRadius(outerradius);
	ShaderLightMapper::instance().addLight(sl);
	// 2-Left
	sl = new SpotLight();
	sl->position(Vector(-3.5f, 15.f, 15.4f));
	sl->color(c);
	sl->direction(Vector(1, -5, 0));
	sl->innerRadius(innerradius);
	sl->outerRadius(outerradius);
	ShaderLightMapper::instance().addLight(sl);
	// 3-Left
	sl = new SpotLight();
	sl->position(Vector(-3.5f, 15.f, 5.4f));
	sl->color(c);
	sl->direction(Vector(1, -5, 0));
	sl->innerRadius(innerradius);
	sl->outerRadius(outerradius);
	ShaderLightMapper::instance().addLight(sl);
	// 4-Left 
	sl = new SpotLight();
	sl->position(Vector(-3.5f, 15.f, -4.1f));
	sl->color(c);
	sl->direction(Vector(1, -5, 0));
	sl->innerRadius(innerradius);
	sl->outerRadius(outerradius);
	ShaderLightMapper::instance().addLight(sl);
	// 5-Left 
	sl = new SpotLight();
	sl->position(Vector(-3.5f, 15.f, -14.1f));
	sl->color(c);
	sl->direction(Vector(1, -5, 0));
	sl->innerRadius(innerradius);
	sl->outerRadius(outerradius);
	ShaderLightMapper::instance().addLight(sl);
	// 6-Licht 
	sl = new SpotLight();
	sl->position(Vector(-3.5f, 15.f, -23.6f));
	sl->color(c);
	sl->direction(Vector(1, -5, 0));
	sl->innerRadius(innerradius);
	sl->outerRadius(outerradius);
	ShaderLightMapper::instance().addLight(sl);
}

/*
 Erzeugt die einzelnen Wasserelemente, welche das gesamte Wasser abbilden.
*/
void Application::createWater() {
	for (int i = 0; i < 3; ++i) {
		this->water = new Water();
		this->water->shader(new PhongShader(), true);
		this->water->loadModel(ASSET_DIRECTORY "Models/water/water.obj", i);
		this->WaterElements.push_back(this->water);
		Models.push_back(this->water);
	}
}

/*
 Schleife welche jedes einzelne Wasserelement updated.
*/
void Application::updateWaterElements(float dtime) {
	// update all WaterElements
	for (auto const& i : this->WaterElements)
	{
		i->update(dtime);
	}
}