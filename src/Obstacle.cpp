#include "Obstacle.h"
#include <time.h>

#define M_PI 3.14159265358979323846

/*
 Konstruktor setzt Startgeschwindigkeit und initialisiert Id und Seed
*/
Obstacle::Obstacle() : ObstacleModel(NULL)
{
	this->speed = 7.f;

	srand(static_cast <unsigned> (time(0)));
}

/*
 Destruktor
*/
Obstacle::~Obstacle()
{
	delete this->ObstacleModel;
}

/*
 Lädt das Modell und fügt es an die Benötigte Position ein.
 Durch die ObstacleId wird garantiert, dass die Distanz zwischen
 den existierenden Autos dem Spieler die Möglichkeit zum Ausweichen bietet.
*/
bool Obstacle::loadModels(const char* ObstacleFile, int ObstacleId)
{
	ObstacleModel = new Model(ObstacleFile, false);
	ObstacleModel->shader(this->pShader, true);

	// save ObstacleId to set fixed distances between obstacles
	this->ObstacleId = ObstacleId;


	// create randomized obstacle position in x
	for (int i = 0; i < ObstacleId; ++i) {
		this->xPosition = this->randomFloat(-1.8f, 2.95f);
	}

	// create gap between obstacle positions in z
	this->zPosition = -10 - (20 * ObstacleId++);

	// apply obstacle position
	Matrix translate, transform;
	translate.translation(xPosition, 5.4, this->zPosition);
	transform = translate * this->ObstacleModel->transform();
	this->ObstacleModel->transform(transform);

	return true;
}

/*
 Bewegt die Autos entlang der Z-Achse und setzt sie zurück sobald
 sie das Ende der Strecke erreichen
*/
void Obstacle::update(float dtime)
{
	// if an obstacle reaches the end of a level, move it back to the beginning
	if (this->ObstacleModel->transform().m23 >= 32.5f) {
		this->reset();
		if (this->speed < 12.f) {
			this->speed += 0.15f;
		}
		std::cout << this->speed;
	}

	Matrix transformMatrix = this->ObstacleModel->transform();
	Matrix translationMatrix;

	// translation
	Vector forward = transformMatrix.forward();
	forward = forward * dtime * this->speed;

	translationMatrix.translation(forward);
	transformMatrix *= translationMatrix;

	// apply transform to everything
	this->ObstacleModel->transform(transformMatrix);
	this->transform(transformMatrix);
}

/*
  Standard Draw-Methode
 */
void Obstacle::draw(const BaseCamera& Cam)
{
	this->ObstacleModel->draw(Cam);
}

/*
 Setzt alle Autos auf ihre initiale Position zurück
 sobald das Spiel verloren wurde.
*/
void Obstacle::gameReset()
{

	// reset truck to beginning of level
	Matrix transformMatrix = this->ObstacleModel->transform();
	Matrix translationMatrix;

	// create randomized obstacle position in x
	this->xPosition = this->randomFloat(-1.8f, 2.95f);
	this->zPosition = -(20 * ObstacleId++);
	Vector spawn = Vector(this->xPosition, 5.4f, this->zPosition);
	translationMatrix.translation(spawn);
	transformMatrix = translationMatrix;

	this->ObstacleModel->transform(transformMatrix);
	this->transform(transformMatrix);

}

/*
 Setzt ein Auto zurück an seine Startposition zurück und
 weist ihnen einen zufälligen X-Wert zu.
*/
void Obstacle::reset()
{

	// set new randomized truck position in x
	this->xPosition = this->randomFloat(-1.8f, 2.95f);

	// apply truck position
	Matrix m;
	m = this->ObstacleModel->transform();
	this->zPosition = -90.f - this->randomFloat(10, 20);
	m.translation(this->xPosition, 5.4f, this->zPosition);
	this->ObstacleModel->transform(m);
	this->transform(m);

}

/*
 Basis GetModel-Methode
*/
const Model* Obstacle::getModel() const
{
	return this->ObstacleModel;
}

/*
 Gibt einen Randomfloat zurück um die Zufallsposition auf der X-Achse zu setzen
*/
const float Obstacle::randomFloat(int min, int max) {
	float random = min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
	return random;
}
