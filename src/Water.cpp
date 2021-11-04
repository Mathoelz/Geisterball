#include "Water.h"

#include <iostream>
#include "PhongShader.h"
using namespace std;

/*
 Konstruktor, initialisiert die Geschwindigkeit
*/
Water::Water() {
    this->speed = 0.5f;
}

/*
 Destruktor
*/
Water::~Water() {
    delete WaterModel;
}

/*
 Lädt das Modell und weist ihm die benötigte Position zu.
*/
bool Water::loadModel(const char* model, int pieceId) {
    PhongShader* pPhongShader;
    // Solange Filenames nicht NULL und nicht leer sind
    if ((model != NULL) && (model[0] != '\0')) {
        this->WaterModel = new Model(model, false);
        pPhongShader = new PhongShader();
        this->WaterModel->shader(pPhongShader, true);
        this->pieceId = pieceId;

        Matrix m, m1, m2;


        m.rotationY(1.56);
        m1.scale(4);
        m2.translation(-1.31 + 1 * this->pieceId, -5, 0 - 116.f * this->pieceId);


        this->WaterModel->transform(m2 * m * m1);

        return true;
    }
    return false;
}

/*
 Bewegt das Wasser entlang der Z-Achse und setzt es zurück sobald
 es den Sichtbaren Bereich der Kamera verlässt.
*/
void Water::update(float dtime) {
    // move the Water till it reaches the end and then reset it

    // Hat die Brücke bereits das Ende erreicht?
    if (this->WaterModel->transform().m23 >= 75.f) {
        this->reset();
    }

    Matrix transformMatrix = this->WaterModel->transform();
    Matrix translationMatrix;

    //translation
    Vector forward = transformMatrix.backward();
    forward = forward * dtime * speed;

    translationMatrix.translation(forward);
    transformMatrix *= translationMatrix;

    this->WaterModel->transform(transformMatrix);
}

/*
 Basis Draw-Methode.
*/
void Water::draw(const BaseCamera& Cam) {
    // Model hat eigene draw Methode
    this->WaterModel->draw(Cam);
}

/*
 Setzt das Wasser an den Anfang der Strecke sobald es
 den Sichtbaren Bereich der Kamera verlässt.
*/
void Water::reset() {

    // reset Water to beginning of level
    Matrix transformMatrix = this->WaterModel->transform();
    Matrix translation, rotation, scale;

    rotation.rotationY(1.56f);
    scale.scale(4);
    translation.translation(65.f, -1.5, 0);

    transformMatrix = rotation * scale * translation;

    this->WaterModel->transform(transformMatrix);
    this->transform(transformMatrix);
}
