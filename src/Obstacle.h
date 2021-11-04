#ifndef Obstacle_hpp
#define Obstacle_hpp

#include <stdio.h>
#include "Model.h"
#include "Lights.h"
#include "ShaderLightMapper.h"


class Obstacle : public BaseModel
{
public:
	Obstacle();
	virtual ~Obstacle();
	bool loadModels(const char* ObstacleFile, int position);
	void update(float dtime);
	virtual void draw(const BaseCamera& Cam);
	void gameReset();
	void reset();
	const Model* getModel() const;

protected:
	Model* ObstacleModel;

	float speed;
	int ObstacleId;
	float xPosition;
	float zPosition;

private:
	const float randomFloat(int min, int max);
};

#endif /* Truck_hpp */
