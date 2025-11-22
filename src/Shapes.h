#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include  "ofxAssimpModelLoader.h"
#include "Octree.h"


class Shape {
public:
	Shape() {
	}
	glm::vec3 position = glm::vec3(0, 0, 0);
	glm::vec3 velocity = glm::vec3(0, 0, 0);
	glm::vec3 acceleration = glm::vec3(0, 0, 0);
	//in degrees
	float rotation = 0.0;
	float rotationalVelocity = 0.0;
	float rotationalAcceleration = 0.0;
	float damping = 0.99;

	ofVec3f forces = ofVec3f(0,0,0);
	float rotationalForce = 0.0;

	virtual void draw() = 0;
	virtual bool inside(glm::vec3 p) = 0;
	glm::vec3 scale = glm::vec3(1.0, 1.0, 1.0);
	glm::mat4 getTransform() {
		glm::mat4 T = glm::translate(glm::mat4(1.0), position);
		glm::mat4 R = glm::rotate(glm::mat4(1.0), glm::radians(rotation), glm::vec3(0, 1, 0));
		glm::mat4 S = glm::scale(glm::mat4(1.0), scale);
		//applied right to left
		return (T * R * S);
	}
};

class Vehicle : public Shape {
public:
	glm::vec3 heading = glm::vec3(1, 0, 0);
	ofxAssimpModelLoader model;
	vector<Box> bboxList;

	void draw() override {

		model.drawFaces();

	}
	void integrate() {
		if (ofGetFrameRate() > 0) {
			// start with any acceleration already on the triangle
			ofVec3f accel = acceleration;
			//add gravity to the acceleration
			accel += ofVec3f(0, -10, 0);
			accel += forces;
			velocity += accel * (1 / ofGetFrameRate());
			velocity *= damping;
			glm::vec3 newPosition = model.getPosition();
			newPosition += velocity * (1 / ofGetFrameRate());

			//we use the assimpmodelloader to store the position
			model.setPosition(newPosition.x, newPosition.y, newPosition.z);

			//adjust rotation
			float rotationalAccel = rotationalAcceleration;
			rotationalAccel += rotationalForce;
			rotationalVelocity += rotationalAccel * (1 / ofGetFrameRate());
			rotationalVelocity *= damping;
			float newRotation = model.getRotationAngle(0);
			newRotation += rotationalVelocity * (1 / ofGetFrameRate());

			//we use the assimpmodelloader to store the rotation as well
			model.setRotation(0, newRotation, 0, 1, 0);

			//reset forces
			forces.set(0, 0, 0);
			rotationalForce = 0;
		}
	}
	bool inside(glm::vec3 p) {
		return false;
	}
};


//  Pure Virtual Function Class - must be subclassed to create new forces.
//
class ShapeForce {
public:
	virtual void updateForce(Shape *) = 0;
};

//adopt the force classes professor gave for my shapes
class TurbulenceShapeForce : public ShapeForce {
	ofVec3f tmin, tmax;

public:
	TurbulenceShapeForce(const ofVec3f & min, const ofVec3f & max);
	void updateForce(Shape * shape) {
		//
		// We are going to add a little "noise" to a particles
		// forces to achieve a more natual look to the motion
		//
		shape->forces.x += ofRandom(tmin.x, tmax.x);
		shape->forces.y += ofRandom(tmin.y, tmax.y);
		shape->forces.z += ofRandom(tmin.z, tmax.z);
	}
};

class ThrustShapeForce : public ShapeForce {
	ofVec3f force;

public:
	ThrustShapeForce(const ofVec3f & force);
	void updateForce(Shape * shape) {
		shape->forces += force;
	}
};

class RotationalShapeForce : public ShapeForce {
	float force;

public:
	RotationalShapeForce(const float & rotation);
	void updateForce(Shape * shape) {
		shape->rotationalForce += force;
	}
};
