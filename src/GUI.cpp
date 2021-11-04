//
//  GUI.cpp
//  ogl4
//
//  Created by Philipp Lensing on 21.09.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#include "GUI.h"
#include "PhongShader.h"

#include <iostream>

#ifdef WIN32
#define ASSET_DIRECTORY "../../assets/"
#else
#define ASSET_DIRECTORY "../assets/"
#endif



GUI::GUI()
{
	this->startMenu = new Model(ASSET_DIRECTORY "Models/menus/startmenu/startmenu.obj", false, 0.0025f);
	this->startMenu->shader(new PhongShader(), true);

	this->pauseMenu = new Model(ASSET_DIRECTORY "Models/menus/pausemenu/pausemenu.obj", false, 0.0025f);
	this->pauseMenu->shader(new PhongShader(), true);

	this->gameOverMenu = new Model(ASSET_DIRECTORY "Models/menus/gameovermenu/gameovermenu.obj", false, 0.0025f);
	this->gameOverMenu->shader(new PhongShader(), true);
}

GUI::~GUI()
{
	delete startMenu;
	delete pauseMenu;
	delete gameOverMenu;
}


bool GUI::load()
{
	return false;
}

void GUI::update(float dtime) {

}


void GUI::draw(const BaseCamera& Cam)
{

	Matrix view;
	SimpleCamera siCam;
	Matrix proj = Cam.getProjectionMatrix();
	siCam.setProjectionMatrix(proj);

	//Matrix rs = menu->transform();

	siCam.setViewMatrix(view.orthographic(1.0f, 1.0f, 1.0f, 2.0f));
	view.invert();
	//view.lookAt(rs.translation(), rs.up(), rs.translation() - rs.forward() + rs.up() * 0.5f);
	//
	//siCam.setViewMatrix(view);

	if (this->startMenuIsActive) {
		this->startMenu->draw(siCam);
	}
	if (this->pauseMenuIsActive) {
		this->pauseMenu->draw(siCam);
	}
	if (this->gameOverMenuIsActive) {
		this->gameOverMenu->draw(siCam);
	}
}



