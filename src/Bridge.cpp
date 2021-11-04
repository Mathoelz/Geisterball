#include "Bridge.h"

#include <iostream>
#include "PhongShader.h"
using namespace std;

/*
 Basis Konstruktor
*/
Bridge::Bridge() {
	this->speed = 0.5f;
}

/*
 Destruktor
*/
Bridge::~Bridge() {
    delete BridgeModel;
}

/*
 Lädt das Modell und fügt es in die benötigte Position ein
*/
bool Bridge::loadModel(const char* model, int pieceId) {
    PhongShader* pPhongShader;
    // Solange Filenames nicht NULL und nicht leer sind
    if ((model != NULL) && (model[0] != '\0')) {
        this->BridgeModel = new Model(model, false);
        pPhongShader = new PhongShader();
        this->BridgeModel->shader(pPhongShader, true);
        this->pieceId = pieceId;

        Matrix m,m1, m2;


        m.rotationY(1.56);
        m1.scale(2.75);
        m2.translation(-0.6f + 0.8f * this->pieceId, 0, 0 - 70.f * this->pieceId);


        this->BridgeModel->transform(m2 * m * m1);

        return true;
    }
    return false;
}

/*
 Draw Methode
*/
void Bridge::draw(const BaseCamera& Cam) {
    // Model hat eigene draw Methode
    this->BridgeModel->draw(Cam);
}
