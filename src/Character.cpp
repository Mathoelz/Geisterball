
#include "Character.h"

#include <iostream>
#include "PhongShader.h"
using namespace std;


/*
 Konstruktor
*/
Character::Character() :PlayerCharacter(NULL), LeftRight(.0f), characterAngle(.0f), drivenWay(0, 0, 0)
{
    this->position = Vector(0, 0, 0);
}

/*
 Destruktor
*/
Character::~Character()
{
    delete PlayerCharacter;
}

/*
 Methode zum Laden des Modells des Playercharacters
*/
bool Character::loadModel(const char* playerModel)
{
    PhongShader* pPhongShader;
    // Solange Filenames nicht NULL und nicht leer sind
    if ((playerModel != NULL) && (playerModel[0] != '\0')) {

        //
        this->PlayerCharacter = new Model(playerModel, false);

        pPhongShader = new PhongShader();
        this->PlayerCharacter->shader(pPhongShader, true);

        Matrix modelviewMatrix = this->PlayerCharacter->transform();
        modelviewMatrix.m[12] = 0.5f;
        modelviewMatrix.m[13] = 5.8f;
        modelviewMatrix.m[14] = 20.0f;
        this->PlayerCharacter->transform(modelviewMatrix);


        return true;
    }
    return false;
}

/*
 Methode welche die Steuerung des Spielers ausführt
*/
void Character::update(float dtime)
{
    Matrix tb_1, tb_2, modelviewMatrix;

    Vector dir = Vector(
        cos(characterAngle),
        0,
        sin(characterAngle)
    );

    if (this->LeftRight != 0) {
        drivenWay = dir * this->LeftRight * 0.01f;
        // if character moves to left
        if (LeftRight < 0) {
            tb_1.rotationZ(0.01);
        }
        // else character moves to right
        else {
            tb_1.rotationZ(-0.01);
        }
    }
    else {
        drivenWay = Vector(0, 0, 0);
        tb_1.rotationZ(0.f);
    }

    tb_2.translation(drivenWay);

    modelviewMatrix = tb_2 * this->PlayerCharacter->transform() * tb_1;
    modelviewMatrix.m[13] = 5.8f;
    modelviewMatrix.m[14] = 23.0f;

    this->PlayerCharacter->transform(modelviewMatrix);

    Vector curPosition = Vector(modelviewMatrix.m[12], modelviewMatrix.m[13], modelviewMatrix.m[14]);
    this->setCurrentPosition(curPosition);

    this->steer(0);
}

/*
 Setzt den Spieler an seine initiale Position zurück,
 sobald das Spiel verloren wurde.
*/
void Character::gameReset() {
    Matrix spawn = this->PlayerCharacter->transform();
    spawn.translation(0, 5.8f, 20.f);
    this->PlayerCharacter->transform(spawn);
    this->transform(spawn);
}

/*
 Kollisionsabfrage des Characters mit den Autos.
*/
bool Character::checkCollision(const Model* model) {

    if (this->PlayerCharacter->transform().translation().X - 1.f < model->transform().translation().X + 0.55f
        && this->PlayerCharacter->transform().translation().X + 1.f > model->transform().translation().X - 0.55f) {

        if (this->PlayerCharacter->transform().translation().Z - 0.8f < model->transform().translation().Z + 1.3f
            && this->PlayerCharacter->transform().translation().Z + 0.5f > model->transform().translation().Z - 1.3f) {

            return true;
        }
    }
    return false;
}

